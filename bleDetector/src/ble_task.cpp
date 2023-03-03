#include <Arduino.h>
#include <esp_log.h>
#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include "ble_task.h"

// global variables
QueueHandle_t bleQueue;
BLEScan *pBLEScan;

// constant definitions
#define TAG             "BLE TASK"
#define SCAN_TIME       1

// static functions
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    /*** Only a reference to the advertised device is passed now
      void onResult(BLEAdvertisedDevice advertisedDevice) { **/
    void onResult(BLEAdvertisedDevice *advertisedDevice) {
        ESP_LOGW(TAG, "BLE Advertised Device found: %s", advertisedDevice->toString().c_str());
        ESP_LOGW(TAG, "BLE RSSI: %d", advertisedDevice->getRSSI());
    }
};

// BLE task handler
void ble_task_handler(void *pvParameters) {
    // setup:
    // - initialize BLE
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99); // less or equal setInterval value

    while(1) {
        // loop: 
        // - scan for BLE devices
        BLEScanResults foundDevices = pBLEScan->start(SCAN_TIME, false);
        // - if device found, send message to MQTT task
        static uint32_t buf = 0;
        buf++;
        //xQueueSend(bleQueue, (void *)&buf, 0);
        pBLEScan->stop();
        pBLEScan->clearResults(); 
        ESP_LOGD(TAG, "BLE task running");
        vTaskDelay(7000 / portTICK_PERIOD_MS);
    }
}