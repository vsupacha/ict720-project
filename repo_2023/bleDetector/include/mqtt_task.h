#ifndef _MQTT_TASK_H_
#define _MQTT_TASK_H_

// include files
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// global variables

// constant definitions

// function prototypes
void mqtt_task_handler(void *pvParameters);

#endif /* _MQTT_TASK_H_ */