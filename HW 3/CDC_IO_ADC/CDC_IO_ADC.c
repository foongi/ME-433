#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define LED_PIN 4
#define BUTTON_PIN 8
#define ADC_PIN 26

int main() {
    stdio_init_all();

    // Setup LED Pin
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Setup Button Pin
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);

    adc_init(); // init the adc module
    adc_gpio_init(ADC_PIN); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    while (!stdio_usb_connected()) { // Very helpful to wait till putty connects
        sleep_ms(100);
    }
    printf("Start!\n");
 
    gpio_put(LED_PIN, 0);

    while(1) { // Since this is a blocking button, use polling
        
        if(!gpio_get(BUTTON_PIN)) { // Hopefully debounce the button a bit 

            sleep_ms(20);
            if(!gpio_get(BUTTON_PIN)) {
                break;
            }
        } 
    }

    while(1) {
         
        gpio_put(LED_PIN, 1);
        printf("Enter a number of analog samples to take (between 1 and 100): \n");
        
        char message[100];
        scanf("%s", message);

        int num_samples = atoi(message);

        int samples[100];

        for(int i = 0; i < num_samples; i++) {
            
            samples[i] = adc_read();
            sleep_ms(10);
        }

        for(int i = 0; i < num_samples; i++) {
            printf("Volts (Sample #%d) \t: %lf\n", i, samples[i] * 3.3/4096);
        }
    }
}