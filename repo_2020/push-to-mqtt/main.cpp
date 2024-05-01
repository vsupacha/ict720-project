#include <mbed.h>
#include <MQTTClientMbedOs.h>

DigitalOut led(LED1);
InterruptIn button(USER_BUTTON);
WiFiInterface *wifi;
TCPSocket* socket;
MQTTClient* mqttClient;
Thread t;
EventQueue queue(5 * EVENTS_EVENT_SIZE);

const char CLIENT_ID[] = "ee67742f-4f46-48a9-a5c9-bb9686a9f273";
const char NETPIE_TOKEN[] = "Su8cjL3tuMaLonksFMbKayLyDd6mZQEA"; 
const char NETPIE_SECRET[] = "Rw-(LA*A$ElI.HZog(Kh&ml#CIm~ZvY6";
const char MQTT_TOPIC[] = "@msg/taist2020/button/1";

void pressed_handler() {
    static int count = 0;
    int ret;
    MQTT::Message message;
 
    // QoS 0
    char buf[100];
    sprintf(buf, "{\"count\":%d}", ++count);
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;
    printf("Sending MQTT message\n");
    ret = mqttClient->publish(MQTT_TOPIC, message);
    if (ret != 0) {
        printf("rc from publish was %d\r\n", ret);
        return;
    }    
}

int main() { 
    // WiFi connection
    wifi = WiFiInterface::get_default_instance();
    if (!wifi) {
        printf("ERROR: No WiFiInterface found.\n");
        return -1;
    }
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        printf("\nConnection error: %d\n", ret);
        return -1;
    }

    // Socket connection
    socket = new TCPSocket();
    socket->open(wifi);
    SocketAddress addr;
    wifi->gethostbyname("mqtt.netpie.io", &addr);
    addr.set_port(1883);
    socket->connect(addr);
    if (ret != 0) {
        printf("rc from TCP connect is %d\r\n", ret);
        return -1;
    }

    // MQTT connection
    mqttClient = new MQTTClient(socket); 
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    //data.MQTTVersion = 3;
    data.clientID.cstring = (char*)CLIENT_ID;
    data.username.cstring = (char*)NETPIE_TOKEN;
    //data.password.cstring = (char*)NETPIE_SECRET;
    ret = mqttClient->connect(data);
    if (ret != 0) {
        printf("rc from MQTT connect is %d\r\n", ret);
        return -1;
    }
 
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    button.fall(queue.event(pressed_handler));
    printf("Starting\n");
    while(1) {
        led = !led;
        ThisThread::sleep_for(500);
    }
}