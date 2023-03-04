#include <Arduino.h>
#include <esp_log.h>
#include "ble_task.h"

// global variables
QueueHandle_t bleQueue;
BLEScan *pBLEScan;

// constant definitions
#define TAG             "BLE TASK"
#define SCAN_TIME       3

// static functions

// BLE task handler
void ble_task_handler(void *pvParameters) {
    // setup:
    // - initialize BLE
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); //create new scan
    // change callback to completion to avoid congestion
    //pBLEScan->setAdvertisedDeviceCallbacks(new advertisedDeviceFoundCallbacks());
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(50); // less or equal setInterval value

    while(1) {
        // loop:
        ble_msg_t msg;
        // - scan for BLE devices
        BLEScanResults foundDevices = pBLEScan->start(SCAN_TIME, false);
        pBLEScan->stop();
        // - if device found, send message to MQTT task
        for (int i=0; i < foundDevices.getCount(); i++) {
            BLEAdvertisedDevice dev = foundDevices.getDevice(i);
            msg.status = 0;
            msg.addr = dev.getAddress();
            msg.rssi = dev.getRSSI();
            xQueueSend(bleQueue, (void *)&msg, 0);
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        // found devices must be cleared at the end to avoid memory corruption
        pBLEScan->clearResults(); 
        vTaskDelay(7000 / portTICK_PERIOD_MS);
    }
}