#include <mbed.h>

DigitalOut led(LED1);
InterruptIn button(USER_BUTTON);
Thread t;
EventQueue queue(5 * EVENTS_EVENT_SIZE);
Timer stopwatch;
Serial pc(USBTX, USBRX);

void pressed_handler() {
    stopwatch.start();
}

void released_handler() {
    stopwatch.stop();
    float elapsed_time = stopwatch.read();
    pc.printf("Time = %f\n", elapsed_time);
}


int main() {
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    button.fall(queue.event(pressed_handler));
    button.rise(queue.event(released_handler));
    pc.printf("Starting\n");
    while(1) {
        led = !led;
        ThisThread::sleep_for(500);
    }
}