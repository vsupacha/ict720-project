import os
import sys
from argparse import ArgumentParser

from flask import Flask, request, abort, jsonify
from flask_mqtt import Mqtt
from pyngrok import ngrok

from linebot import (
    LineBotApi, WebhookHandler
)
from linebot.exceptions import (
    InvalidSignatureError
)
from linebot.models import (
    MessageEvent, TextMessage, TextSendMessage,
)

from pymongo import MongoClient
from datetime import datetime
import json

# database code
mongoClient = MongoClient(os.environ['MONGODB_URI'], 27017)
if os.environ["PHASE"] == "DEVELOPMENT":
    app_db = mongoClient.blecollector_test # Database
if os.environ["PHASE"] == "PRODUCTION":
    app_db = mongoClient.blecollector # Database

app = Flask(__name__, static_url_path='/ui', static_folder='web/')
app.config['MQTT_BROKER_URL'] = 'broker.hivemq.com'
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_REFRESH_TIME'] = 1.0  # refresh time in seconds
mqtt = Mqtt(app)

# get channel_secret and channel_access_token from your environment variable
channel_secret = os.getenv('LINE_CHANNEL_SECRET', None)
channel_access_token = os.getenv('LINE_CHANNEL_ACCESS_TOKEN', None)
if channel_secret is None:
    print('Specify LINE_CHANNEL_SECRET as environment variable.')
    sys.exit(1)
if channel_access_token is None:
    print('Specify LINE_CHANNEL_ACCESS_TOKEN as environment variable.')
    sys.exit(1)

line_bot_api = LineBotApi(channel_access_token)
handler = WebhookHandler(channel_secret)

@app.route("/", methods=['GET'])
def hello_world():
    return "Hello, World!"

@app.route("/api", methods=['GET'])
def api_func():
    userId = request.args.get('userId');
    user_col = app_db.user # Collection
    ans = user_col.find_one({"userId": userId})
    logging_col = app_db.logging # Collection
    ans = logging_col.find_one({"ble": ans['ble']})
    location_col = app_db.location # Collection
    ans = location_col.find_one({"detector_id": ans['detector_id']})
    resp = {"userId": userId, "place":ans['place']}
    return jsonify(resp)

@app.route("/api/inject_user", methods=['POST'])
def inject_user():
    if os.environ["PHASE"] == "DEVELOPMENT":
        data = request.get_json()
        data['timestamp'] = datetime.now()
        user_col = app_db.user # Collection
        user_col.insert_one(data)
        return jsonify({"status": "OK"})
    return jsonify({"status": "ERROR"})

@app.route("/api/query_user", methods=['GET'])
def query_user():
    if os.environ["PHASE"] == "DEVELOPMENT":
        userId = request.args.get('userId')
        user_col = app_db.user # Collection
        ans = user_col.find_one({"userId": userId})
        resp = {"userId": userId, "ble":ans['ble']}
        return jsonify(resp)
    return jsonify({"status": "ERROR"})

@app.route("/api/inject_logging", methods=['POST'])
def inject_logging():
    if os.environ["PHASE"] == "DEVELOPMENT":
        data = request.get_json()
        data['timestamp'] = datetime.now()
        logging_col = app_db.logging # Collection
        logging_col.insert_one(data)
        return jsonify({"status": "OK"})
    return jsonify({"status": "ERROR"})

@app.route("/api/inject_location", methods=['POST'])
def inject_location():
    if os.environ["PHASE"] == "DEVELOPMENT":
        data = request.get_json()
        data['timestamp'] = datetime.now()
        location_col = app_db.location # Collection
        location_col.insert_one(data)
        return jsonify({"status": "OK"})
    return jsonify({"status": "ERROR"})

@app.route("/callback", methods=['POST'])
def callback():
    # get X-Line-Signature header value
    signature = request.headers['X-Line-Signature']

    # get request body as text
    body = request.get_data(as_text=True)
    print(body)
    app.logger.info("Request body: " + body)

    # handle webhook body
    try:
        handler.handle(body, signature)
    except InvalidSignatureError:
        abort(400)

    return 'OK'

@handler.add(MessageEvent, message=TextMessage)
def message_text(event):
    line_bot_api.reply_message(
        event.reply_token,
        TextSendMessage(text=event.message.text + ', me too')
    )

@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    mqtt.subscribe(os.environ['DATA_TOPIC'])

@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    data = json.loads( message.payload.decode('ascii') )
    data['timestamp'] = datetime.now()
    logging_col = app_db.logging # Collection
    logging_col.insert_one(data)

if __name__ == "__main__":
    arg_parser = ArgumentParser(
        usage='Usage: python ' + __file__ + ' [--port <port>] [--help]'
    )
    arg_parser.add_argument('-p', '--port', default=8000, help='port')
    arg_parser.add_argument('-d', '--debug', default=False, help='debug')
    options = arg_parser.parse_args()

    # NGROK and flask
    ngrok.set_auth_token(os.environ['NGROK_TOKEN'])
    public_url = ngrok.connect(options.port)
    url = public_url.public_url.replace('http', 'https') + '/callback'
    print(url)
    line_bot_api.set_webhook_endpoint(url)
    app.run(debug=options.debug, port=options.port, host='0.0.0.0', use_reloader=False)