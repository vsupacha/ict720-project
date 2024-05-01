#include <mbed.h>

DigitalOut led(LED1);
InterruptIn button(USER_BUTTON);
Thread t;
EventQueue queue(5 * EVENTS_EVENT_SIZE);

void pressed_handler() {
    led = 1;
}

void released_handler() {
    led = 0;
}

int main() {
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    button.fall(queue.event(pressed_handler));
    button.rise(queue.event(released_handler));
    while(1) {
        ThisThread::sleep_for(100);
    }
}
