#include <Arduino.h>
#include <esp_log.h>
#include "ble_task.h"
#include "mqtt_task.h"

// static variables

// static functions

// constant definitions
#define TAG             "MQTT TASK"

// MQTT task handler
void mqtt_task_handler(void *pvParameters) {
    // setup:
    // - connect to wifi
    // - connect to mqtt broker
    // - subscribe to topics

    // loop: 
    while(1) {
        // wait for message from BLE task
        // - if message received, publish to mqtt broker
        ble_msg_t buf;
        xQueueReceive(bleQueue, &buf, portMAX_DELAY);
        ESP_LOGW(TAG, "MQTT task running");
        ESP_LOGW(TAG, "Queue received %s: %d", buf.addr.toString().c_str(), buf.rssi);
    }
}