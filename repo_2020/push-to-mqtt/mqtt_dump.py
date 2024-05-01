import paho.mqtt.subscribe as subscribe

CLIENT_ID = 'ID from NETPIE'
NETPIE_TOKEN = 'token from NETPIE'

if __name__ == '__main__':
    while True:
        msg = subscribe.simple('@msg/taist2020/button/#', hostname='mqtt.netpie.io', port=1883, client_id=CLIENT_ID, auth={'username':NETPIE_TOKEN, 'password':None}, keepalive=10)
        print("%s %s" % (msg.topic, msg.payload))