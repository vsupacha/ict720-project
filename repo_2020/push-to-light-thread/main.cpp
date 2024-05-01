#include <mbed.h>

DigitalOut led(LED1);
DigitalIn button(USER_BUTTON);
Thread led_thread;

void button_handler() {
    while(1) {
        if (!button) {
            led = 1;
        } else {
            led = 0;
        }
        ThisThread::sleep_for(100);
    }
}

int main() {
    led_thread.start(&button_handler);
    while(1) {
        ThisThread::sleep_for(1000);
    }
}