/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"

#define FLAG_VALUE 123

#define VOLTAGE_FLAG 000
#define LED_ON_FLAG 111
#define LED_OFF_FLAG 222

#define CORE1_RETURN_FLAG 321

#define LED_PIN 15

uint16_t adc0_voltage; 

void pico_led_init(void) {
    #if defined(LED_PIN)
        // init LED pin and set relevant registers
        gpio_init(LED_PIN);
        gpio_set_dir(LED_PIN, GPIO_OUT);
    #endif
}


void pico_set_led(bool led_on) {
    #if defined(LED_PIN)
        // Just set the GPIO on or off
        gpio_put(LED_PIN, !led_on);
    #endif
    }


void core1_entry() {
    pico_led_init();

    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    // multicore_fifo_push_blocking(FLAG_VALUE);

    // uint32_t g = multicore_fifo_pop_blocking();

    // if (g != FLAG_VALUE)
    //     printf("Hmm, that's not right on core 1!\n");
    // else
    //     printf("Its all gone well on core 1!");

    while (1) {

        uint32_t flag = multicore_fifo_pop_blocking();

        if(flag == VOLTAGE_FLAG) 
        {
            adc0_voltage = adc_read();
            multicore_fifo_push_blocking(CORE1_RETURN_FLAG);

        }
        else if(flag == LED_ON_FLAG)
        {
            pico_set_led(true);
            multicore_fifo_push_blocking(CORE1_RETURN_FLAG);
        }
        else if(flag == LED_OFF_FLAG)
        {
            pico_set_led(false);
            multicore_fifo_push_blocking(CORE1_RETURN_FLAG);
        }
        else 
        {
            printf("Something wrong on core 1\n");
            break;
        }

    }
}

int main() {
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    
    printf("Hello, multicore!\n");
    

    /// \tag::setup_multicore[]

    multicore_launch_core1(core1_entry);

    // Wait for it to start up

    // uint32_t g = multicore_fifo_pop_blocking();

    // if (g != FLAG_VALUE)
    //     printf("Hmm, that's not right on core 0!\n");
    // else {
    //     multicore_fifo_push_blocking(FLAG_VALUE);
    //     printf("It's all gone well on core 0!\n");
    // }

    /// \end::setup_multicore[]


    char message[50];
    int input;

    while(1) 
    {
        
        printf("Enter a number 0, 1, or 2: ");
        scanf("%s", message);
        printf("You entered: %s\n", message);

        input = atoi(message);

        if(input == 0)
        {
            multicore_fifo_push_blocking(VOLTAGE_FLAG);

            uint32_t flag = multicore_fifo_pop_blocking();
        
            if(flag != CORE1_RETURN_FLAG)
            {
                printf("Something wrong on core 0.\n");
                break;
            }

            float volts = ((adc0_voltage * 3.3/(1 << 12)));
            printf("ADC0 Voltage: %lf \n", volts);
        }
        else if (input == 1)
        {
            multicore_fifo_push_blocking(LED_ON_FLAG);

            uint32_t flag = multicore_fifo_pop_blocking();
        
            if(flag != CORE1_RETURN_FLAG)
            {
                printf("Something wrong on core 0.\n");
                break;
            }
        }
        else if (input == 2)
        {
            multicore_fifo_push_blocking(LED_OFF_FLAG);
        
            uint32_t flag = multicore_fifo_pop_blocking();
        
            if(flag != CORE1_RETURN_FLAG)
            {
                printf("Something wrong on core 0.\n");
                break;
            }
        }



    }

}
