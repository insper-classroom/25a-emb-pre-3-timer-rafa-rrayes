#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <stdbool.h>

#define BTN_PIN_R 28
#define LED_PIN_R 4

volatile bool flag_timer_triggered = false;
volatile bool flag_button_pressed = false;

repeating_timer_t blink_timer;

bool blink_timer_callback(repeating_timer_t *rt) {
    flag_timer_triggered = true;
    return true;
}

void button_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {
        flag_button_pressed = true;
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    bool blinking = false;

    while (true) {
        // Handle button press
        if (flag_button_pressed) {
            flag_button_pressed = false;

            if (blinking) {
                cancel_repeating_timer(&blink_timer);
                gpio_put(LED_PIN_R, 0);
            } else {
                add_repeating_timer_ms(500, blink_timer_callback, NULL, &blink_timer);
            }

            blinking = !blinking;
        }

        if (flag_timer_triggered) {
            flag_timer_triggered = false;
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
        }

    }
}
