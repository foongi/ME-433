/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

 
#define LED_PIN 4
#define GPIO_WATCH_PIN 8
 
static int presses;
static bool reading;

// Initialize the GPIO for the LED
void pico_led_init(void) {
     #ifdef LED_PIN
         // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
         // so we can use normal GPIO functionality to turn the led on and off
         gpio_init(LED_PIN);
         gpio_set_dir(LED_PIN, GPIO_OUT);
     #endif
     }
 
     // Turn the LED on or off
 void pico_set_led(bool led_on) {
 #if defined(LED_PIN)
    // Just set the GPIO on or off
    gpio_put(LED_PIN, led_on);
 #endif
 }
 
 void gpio_callback(uint gpio, uint32_t events) {
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it


    
    // Debounce Button
    sleep_ms(20);
    if(!gpio_get(GPIO_WATCH_PIN))
    {
    return;
    }

    if (presses % 2 == 0)
    {
        pico_set_led(true);
    } else {
        pico_set_led(false);
    }
    printf("Presses: %d\n", presses);
    presses++;
 }
 
 int main() {
    pico_led_init();
    stdio_init_all();
    
    reading = false;
    presses = 0;

    gpio_init(GPIO_WATCH_PIN);
    gpio_set_irq_enabled_with_callback(GPIO_WATCH_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    
    printf("Hello GPIO IRQ\n");

    while(true){
        
    }
 
 }
 