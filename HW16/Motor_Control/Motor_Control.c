#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"

#define M1EN 27 // Motor 1 PWM
#define M1PH 26 // Motor 1 Phase
#define M2EN 21 // Motor 2 PWM
#define M2PH 20 // Motor 2 Phase
#define WRAP 6250


void setup_motors()
{
    gpio_init(M1PH);
    gpio_set_dir(M1PH, GPIO_OUT);
    gpio_init(M2PH);
    gpio_set_dir(M2PH, GPIO_OUT);


    gpio_set_function(M1EN, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(M1EN); // Get PWM slice number
    float div = 1; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    pwm_set_wrap(slice_num, WRAP);
    pwm_set_enabled(slice_num, true); // turn on the PWM
    pwm_set_gpio_level(M1EN, 0); 



    gpio_set_function(M2EN, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    slice_num = pwm_gpio_to_slice_num(M2EN); // Get PWM slice number
    div = 1; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    pwm_set_wrap(slice_num, WRAP);
    pwm_set_enabled(slice_num, true); // turn on the PWM
    pwm_set_gpio_level(M2EN, 0); 
}

void set_motor1(float duty_cycle)
{
    pwm_set_gpio_level(M1EN, duty_cycle*WRAP); 
    
    if (duty_cycle > 0)
    {
        gpio_put(M1PH, 1);
    }
    else 
    {
        gpio_put(M1PH, 0);
    }


}

void set_motor2(float duty_cycle)
{
    pwm_set_gpio_level(M2EN, duty_cycle*WRAP); 

    if (duty_cycle > 0)
    {
        gpio_put(M2PH, 1);
    }
    else 
    {
        gpio_put(M2PH, 0);
    }
    
}

int main()
{
    stdio_init_all();


    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    setup_motors();


    float duty_cycle = 0.0;
    while (true) {

        printf("Enter + or - to increase or decrease the duty cycle.\n");

        char message[100];
        scanf("%s", message);

        if(strcmp(message,"+") == 0)
        {
            duty_cycle += 0.01;
        } else if (strcmp(message,"-") == 0)
        {
            duty_cycle -= 0.01;
        }

        if(duty_cycle > 1.0)
        {
            duty_cycle = 1.0;
        }
        else if (duty_cycle < -1.0)
        {
            duty_cycle = -1.0;
        }

        printf("Duty Cycle: %f\n", duty_cycle);

        set_motor1(duty_cycle);
        set_motor2(duty_cycle);

    }
}
