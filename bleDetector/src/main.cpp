#include <Arduino.h>
#include <esp_log.h>
#include "ble_task.h"
#include "mqtt_task.h"

// static variables

// static functions

// constant definitions
#define TAG             "MAIN"
#define BLE_TASK_PRIO   2
#define MQTT_TASK_PRIO  3

void setup() {
  ESP_LOGI(TAG, "Starting BLE detector");
  bleQueue = xQueueCreate(BLE_QUEUE_SIZE, sizeof(ble_msg_t));
  xTaskCreate(ble_task_handler, "BLE task", 4096, NULL, BLE_TASK_PRIO, NULL);
  xTaskCreate(mqtt_task_handler, "MQTT task", 4096, NULL, MQTT_TASK_PRIO, NULL);
}

void loop() {
  ESP_LOGW(TAG, "Main task %d: %d", millis(), esp_get_free_heap_size());
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}