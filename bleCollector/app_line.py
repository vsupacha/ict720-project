import os
import sys
from argparse import ArgumentParser

from flask import Flask, request, abort, jsonify
from linebot import (
    LineBotApi, WebhookHandler
)
from linebot.exceptions import (
    InvalidSignatureError
)
from linebot.models import (
    MessageEvent, TextMessage, TextSendMessage,
)
from pyngrok import ngrok

app = Flask(__name__, static_url_path='/ui', static_folder='web/')

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
count = 0

@app.route("/", methods=['GET'])
def hello_world():
    return "Hello, World!"

@app.route("/api", methods=['GET'])
def api_func():
    userId = request.args.get('userId');
    print('User ', userId + 'connected' )
    resp = {"my_txt": "hello, REST " + str(count)} # resp["my_txt"]
    line_bot_api.push_message(userId, TextSendMessage(text='Hello, how are you?'))
    return jsonify(resp)

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


if __name__ == "__main__":
    arg_parser = ArgumentParser(
        usage='Usage: python ' + __file__ + ' [--port <port>] [--help]'
    )
    arg_parser.add_argument('-p', '--port', default=8000, help='port')
    arg_parser.add_argument('-d', '--debug', default=False, help='debug')
    options = arg_parser.parse_args()

    ngrok.set_auth_token(os.environ['NGROK_TOKEN'])
    public_url = ngrok.connect(options.port)
    url = public_url.public_url.replace('http', 'https') + '/callback'
    print(url)
    line_bot_api.set_webhook_endpoint(url)
    app.run(debug=options.debug, port=options.port)