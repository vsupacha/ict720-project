#include <mbed.h>

DigitalOut led(LED1);
InterruptIn button(USER_BUTTON);

void pressed_handler() {
    led = 1;
}

void released_handler() {
    led = 0;
}

int main() {
    button.fall(&pressed_handler);
    button.rise(&released_handler);
    while(1) {
        wait(1.0);
    }
}
