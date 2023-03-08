import paho.mqtt.client as mqtt
import asyncio

class bleCollector:
   def __init__(self):
      self.mqttClient = mqtt.Client()
      self.mqttClient.on_message = self.on_message
      self.running = True

   def on_message(self, client, userdata, message):
      print("Message Recieved: " + message.payload.decode())
   
   async def co_run(self, broker_url, broker_port, sub_topic):
      self.mqttClient.connect(broker_url, broker_port)
      self.mqttClient.subscribe(sub_topic)
      while self.running:
         self.mqttClient.loop()
         await asyncio.sleep(0.1)

   def run(self, broker_url, broker_port, sub_topic):
      asyncio.run(self.co_run(broker_url, broker_port, sub_topic))

if __name__ == '__main__':
   broker_url = "???"
   broker_port = 1883
   sub_topic = "???"
   app = bleCollector()
   app.run(broker_url, broker_port, sub_topic)