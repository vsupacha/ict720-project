#include <mbed.h>

DigitalOut led(LED1);
DigitalIn button(USER_BUTTON);

int main() {
    while(1) {
        if (!button) {
            led = 1;
        } else {
            led = 0;
        }
        
        wait(0.1);
    }
}
