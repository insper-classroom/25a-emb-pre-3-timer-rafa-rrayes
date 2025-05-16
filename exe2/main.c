#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define BTN_PIN_R 28
#define LED_PIN_R 4
#define BTN_PIN_G 26
#define LED_PIN_G 6

volatile bool flag_timer_red = false;
volatile bool flag_timer_green = false;
volatile bool flag_btn_red = false;
volatile bool flag_btn_green = false;

bool counting_red = false;
bool counting_green = false;

repeating_timer_t timer_red;
repeating_timer_t timer_green;

bool timer_red_callback(repeating_timer_t *rt) {
    flag_timer_red = true;
    return true;
}

bool timer_green_callback(repeating_timer_t *rt) {
    flag_timer_green = true;
    return true;
}

void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {
        if (gpio == BTN_PIN_R)
            flag_btn_red = true;
        else if (gpio == BTN_PIN_G)
            flag_btn_green = true;
    }
}

void setup_gpio() {
    // LEDs
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, 0);

    // Botões
    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);
}

void toggle_timer(bool *counting, repeating_timer_t *timer, int led_pin, int interval_ms, 
                  bool (*callback)(repeating_timer_t *)) {
    if (*counting) {
        cancel_repeating_timer(timer);
        gpio_put(led_pin, 0);
    } else {
        add_repeating_timer_ms(interval_ms, callback, NULL, timer);
    }
    *counting = !(*counting);
}

int main() {
    stdio_init_all();
    setup_gpio();

    while (true) {
        if (flag_btn_red) {
            flag_btn_red = false;
            toggle_timer(&counting_red, &timer_red, LED_PIN_R, 500, timer_red_callback);
        }

        if (flag_btn_green) {
            flag_btn_green = false;
            toggle_timer(&counting_green, &timer_green, LED_PIN_G, 250, timer_green_callback);
        }

        if (flag_timer_red) {
            flag_timer_red = false;
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
        }

        if (flag_timer_green) {
            flag_timer_green = false;
            gpio_put(LED_PIN_G, !gpio_get(LED_PIN_G));
        }
    }
}
