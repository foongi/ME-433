#include <stdio.h>
#include <string.h>
#include "cam.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define M1EN 21 // Motor 1 PWM
#define M1PH 20 // Motor 1 Phase
#define M2EN 16 // Motor 2 PWM
#define M2PH 18 // Motor 2 Phase
#define WRAP 12500

#define SETPOINT 40  // Center of the image
#define BASE_SPEED 0.5f
#define KP 0.007f
#define KI 0.0000f
#define KD 0.001f


void setup_motors()
{
    gpio_init(M1PH);
    gpio_set_dir(M1PH, GPIO_OUT);
    gpio_init(M2PH);
    gpio_set_dir(M2PH, GPIO_OUT);

    gpio_pull_down(M1EN);
    gpio_pull_down(M2EN);



    gpio_set_function(M1EN, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    uint slice1 = pwm_gpio_to_slice_num(M1EN); // Get PWM slice number
    float div = 1; // must be between 1-255
    pwm_set_clkdiv(slice1, div); // divider
    pwm_set_wrap(slice1, WRAP);
    pwm_set_enabled(slice1, true); // turn on the PWM
    pwm_set_gpio_level(M1EN, 0); 



    gpio_set_function(M2EN, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    uint slice2 = pwm_gpio_to_slice_num(M2EN); // Get PWM slice number
    div = 1; // must be between 1-255
    pwm_set_clkdiv(slice2, div); // divider
    pwm_set_wrap(slice2, WRAP);
    pwm_set_enabled(slice2, true); // turn on the PWM
    pwm_set_gpio_level(M2EN, 0); 
}

void set_motor1(float duty_cycle)
{
    
    if (duty_cycle > 0)
    {
        gpio_put(M1PH, 1);
        pwm_set_gpio_level(M1EN, duty_cycle*WRAP); 
    }
    else 
    {
        gpio_put(M1PH, 0);
        pwm_set_gpio_level(M1EN, -1*duty_cycle*WRAP); 
    }


}

void set_motor2(float duty_cycle)
{

    if (duty_cycle > 0)
    {
        gpio_put(M2PH, 1);
        pwm_set_gpio_level(M2EN, duty_cycle*WRAP); 
    }
    else 
    {
        gpio_put(M2PH, 0);
        pwm_set_gpio_level(M2EN, -1*duty_cycle*WRAP); 

    }
    
}

int main()
{

    init_camera_pins();
    setup_motors(); // Sets up PWM

    float integral = 0;
    float last_error = 0;

    absolute_time_t last_time = get_absolute_time();

    float com_filtered = 0;
    float alpha = 0.3; // smoothing factor, 0 < alpha < 1

    // printf("Starting while loop");
    while (true) {

        setSaveImage(1);
        while(getSaveImage()==1){}
        convertImage();
        int com = findLine(50); // calculate the position of the center of the ine
        // setPixel(IMAGESIZEY/2,com,0,255,0); // draw the center so you can see it in python
        // printImage();

        

        // In your main loop:
        com_filtered = alpha * com + (1 - alpha) * com_filtered;

        float error = SETPOINT - com_filtered;


        // Compute time delta
        absolute_time_t now = get_absolute_time();
        float dt = absolute_time_diff_us(last_time, now) / 1e6f;
        last_time = now;



        // PID terms
        integral += error * dt;
        float derivative = (error - last_error) / dt;
        last_error = error;

        float correction = KP * error + KI * integral + KD * derivative;

        // Calculate motor duty cycles
        float left_speed = BASE_SPEED - correction;
        float right_speed = BASE_SPEED + correction;

        // Clamp to [-1, 1]
        if (left_speed > 1.0f) left_speed = 1.0f;
        if (left_speed < -1.0f) left_speed = -1.0f;
        if (right_speed > 1.0f) right_speed = 1.0f;
        if (right_speed < -1.0f) right_speed = -1.0f;

        set_motor1(left_speed - 0.05);
        set_motor2(right_speed);
    }
}
