#include <Arduino.h>
#include <esp_log.h>
#include "ble_task.h"
#include "mqtt_task.h"

// static variables

// static functions

// constant definitions
#define TAG             "MAIN"
#define BLE_TASK_PRIO   1
#define MQTT_TASK_PRIO  3

void setup() {
  ESP_LOGI(TAG, "Starting BLE detector");
  bleQueue = xQueueCreate(BLE_QUEUE_SIZE, sizeof(uint32_t));
  xTaskCreate(ble_task_handler, "BLE task", 4096, NULL, BLE_TASK_PRIO, NULL);
  xTaskCreate(mqtt_task_handler, "MQTT task", 4096, NULL, MQTT_TASK_PRIO, NULL);
}

void loop() {
  ESP_LOGW(TAG, "Main task %d: %d", uxTaskPriorityGet(NULL), millis());
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}