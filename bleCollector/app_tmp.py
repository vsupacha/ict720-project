import paho.mqtt.client as mqtt
import asyncio
from pymongo import MongoClient
from dotenv import load_dotenv
import os
from datetime import datetime

class bleCollector:
   def __init__(self):
      self.mqttClient = None
      self.mongoClient = None
      self.running = True

   def on_message(self, client, userdata, message):
      print("Message Recieved: " + message.payload.decode())

   async def co_mongo(self):
      self.mongoClient = MongoClient(os.environ['MONGODB_URI'], 27017)
      ble_db = self.mongoClient.blecollector # Database
      hb_collection = ble_db.heart_beat # Collection
      count = 0
      while self.running:
         print("MongoDB is running")
         beat = {"timestamp": datetime.now(), "count": count}
         hb_collection.insert_one(beat)
         count += 1
         await asyncio.sleep(1)
   
   async def co_mqtt(self):
      self.mqttClient = mqtt.Client()
      self.mqttClient.on_message = self.on_message
      self.mqttClient.connect(os.environ["BROKER_URL"], int(os.environ["BROKER_PORT"]))
      self.mqttClient.subscribe(os.environ["DATA_TOPIC"])
      while self.running:
         print("MQTT is running")
         self.mqttClient.loop()
         await asyncio.sleep(1)
   
   async def co_main(self):
      await asyncio.gather(self.co_mqtt(), self.co_mongo())

   def run(self):
      asyncio.run(self.co_main())

if __name__ == '__main__':
   load_dotenv() 
   app = bleCollector()
   app.run()