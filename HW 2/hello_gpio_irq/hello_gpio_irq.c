/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#ifndef PICO_DEFAULT_LED_PIN
#warning blink_simple example requires a board with a regular LED
#endif

#define GPIO_WATCH_PIN 25

static int presses;

// Initialize the GPIO for the LED
void pico_led_init(void) {
    #ifdef PICO_DEFAULT_LED_PIN
        // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
        // so we can use normal GPIO functionality to turn the led on and off
        gpio_init(PICO_DEFAULT_LED_PIN);
        gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    #endif
    }

    // Turn the LED on or off
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#endif
}

void gpio_callback(uint gpio, uint32_t events) {
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it

    if (presses % 2 == 0)
    {
        pico_set_led(true);
    } else {
        pico_set_led(false);
    }
    printf("Presses: %d", presses);
    presses++;
}

int main() {
    pico_led_init();
    stdio_init_all();
    

    presses = 0;

    printf("Hello GPIO IRQ\n");
    // gpio_init(GPIO_WATCH_PIN);
    // gpio_set_irq_enabled_with_callback(GPIO_WATCH_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    while (true) {
        printf("Hello, world!\n");
        pico_set_led(true);
        sleep_ms(1000);
        pico_set_led(false);
        sleep_ms(1000);
    }

}
