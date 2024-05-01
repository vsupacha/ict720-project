# TAIST 2020 repository

## Feb 24 class
Examples to demonstrate mbed's execution flows. All examples are based on push-to-light operation. 
* USER_BUTTON and LED1 are used as hardware
* No additional software library

### push-to-light-polling
Single thread execution in main loop.

### push-to-light-irq
Foreground-background execution using IRQ handler of button.

### push-to-light-thread
Thread execution with polling in separate thread.

### push-to-light-eventqueue
Event queue with separate thread to run EventQueue service, then use IRQ handler to send event. Code is executed in Thread environment. 

## Feb 26 class
Examples to demonstrate mbed's protocol stacks. 
* USER_BUTTON, LED1, and WiFi interface are used as hardware
* [ISM43362 WiFi library](https://github.com/ARMmbed/wifi-ism43362)
* [mbed HTTP library](https://os.mbed.com/teams/sandbox/code/mbed-http/)
* [mbed MQTT library](https://github.com/ARMmbed/mbed-mqtt)

### push-to-serial
Example code for serial port.

### push-to-wifi
Example code for WiFi scanning and joining. Required WiFi personal WPA/WPA2 access point.

### push-to-http
Example code for HTTP connection.

### push-to-https
Example code for HTTPS connection.

### push-to-mqtt
Example code for MQTT connection.
