#include <Arduino.h>
#include <esp_log.h>
#include "ble_task.h"

// global variables
QueueHandle_t bleQueue;

// static functions

// constant definitions
#define TAG             "BLE TASK"

// BLE task handler
void ble_task_handler(void *pvParameters) {
    // setup:
    // - initialize BLE

    while(1) {
        // loop: 
        // - scan for BLE devices
        // - if device found, send message to MQTT task
        static uint32_t buf = 0;
        buf++;
        xQueueSend(bleQueue, (void *)&buf, 0);
        ESP_LOGD(TAG, "BLE task running");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}