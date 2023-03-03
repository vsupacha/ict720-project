#ifndef _BLE_TASK_H_
#define _BLE_TASK_H_

// global variables
extern QueueHandle_t bleQueue;

// constant definitions
#define BLE_QUEUE_SIZE      10

// function prototypes
void ble_task_handler(void *pvParameters);

#endif /* _BLE_TASK_H_ */