#include <Arduino.h>
#include <esp_log.h>
#include "ble_task.h"

// global variables
QueueHandle_t bleQueue;

// constant definitions
#define TAG             "BLE TASK"
#define SCAN_TIME       3

// static functions
static BLEScan *pBLEScan;
static TickType_t prevWaketime = 0;

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
    pBLEScan->setWindow(99); // less or equal Interval value

    while(1) {
        // loop:
        ble_msg_t msg;
        // - scan for BLE devices
        xTaskDelayUntil(&prevWaketime, 10000 / portTICK_PERIOD_MS);
        BLEScanResults foundDevices = pBLEScan->start(SCAN_TIME, false); // scan for SCAN_TIME seconds
        pBLEScan->stop();
        // - if device found, send message to MQTT task
        for (int i=0; i < foundDevices.getCount(); i++) {
            BLEAdvertisedDevice dev = foundDevices.getDevice(i);
            msg.timestamp = millis()/1000;
            msg.addr = dev.getAddress();
            msg.rssi = dev.getRSSI();
            xQueueSend(bleQueue, (void *)&msg, 0);
        }
        // found devices must be cleared at the end to avoid memory corruption
        pBLEScan->clearResults(); 
    }
}