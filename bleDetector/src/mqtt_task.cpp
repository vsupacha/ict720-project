#include <Arduino.h>
#include <esp_log.h>
#include "ble_task.h"
#include "mqtt_task.h"

// constant definitions
const char* SSID = "Vsupacha_2.4G";
const char* PASSWORD = "nokandbee";
const char* MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "Mbits_demo";
const char* PUB_TOPIC = "ict720/vsupacha/data";
const char* SUB_TOPIC = "ict720/vsupacha/cmd";

const int RSSI_THR = -60;

// static variables
static WiFiClient espClient;
static PubSubClient mqttClient(espClient);
static DynamicJsonDocument doc(1024);

// static functions

// constant definitions
#define TAG             "MQTT TASK"

// callback function
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    char buf[256];
    memcpy(buf, payload, length);
    buf[length] = 0;
    deserializeJson(doc, buf);
    if (doc["led"] == "on") {
        ESP_LOGW(TAG, "Got LED ON command");
    }
}

// MQTT task handler
void mqtt_task_handler(void *pvParameters) {
    // setup:
    // - connect to wifi
    WiFi.mode(WIFI_OFF);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    ESP_LOGW(TAG, "WiFi connected: %s", WiFi.localIP().toString().c_str());
    // - connect to mqtt broker
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setCallback(mqtt_callback);
    mqttClient.connect(MQTT_CLIENT_ID);
    // - subscribe to topics
    mqttClient.subscribe(SUB_TOPIC);
    // loop: 
    while(1) {
        // wait for message from BLE task
        // - if message received, publish to mqtt broker
        ble_msg_t ble_msg;
        char msg_buf[256];
        if (xQueueReceive(bleQueue, &ble_msg, 1000/portTICK_PERIOD_MS) == pdTRUE) {
            ESP_LOGW(TAG, "MQTT task running");
            ESP_LOGW(TAG, "Queue received %s: %d", ble_msg.addr.toString().c_str(), ble_msg.rssi);
            if (ble_msg.rssi > -60) {
                doc["timestamp"] = ble_msg.timestamp;
                doc["addr"] = ble_msg.addr.toString().c_str();
                doc["rssi"] = ble_msg.rssi;
                serializeJson(doc, msg_buf);
                mqttClient.publish(PUB_TOPIC, msg_buf);
            }
        }
        mqttClient.loop();
    }
}