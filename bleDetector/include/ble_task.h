#ifndef _BLE_TASK_H_
#define _BLE_TASK_H_
// include files
#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>

// global variables
extern QueueHandle_t bleQueue;

// constant definitions
#define BLE_QUEUE_SIZE      10

// data type definitions
typedef struct {
    int8_t        status;
    NimBLEAddress addr;
    int8_t        rssi;
} ble_msg_t;

// function prototypes
void ble_task_handler(void *pvParameters);

#endif /* _BLE_TASK_H_ */