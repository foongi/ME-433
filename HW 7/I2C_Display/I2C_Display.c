#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

#define ADDR 0b0100000


void MCP_setup(uint8_t address)
{
    uint8_t arr[2] = {0x00, 0b01111111}; // Set gp7 output and gp0 input

    i2c_write_blocking(i2c_default, address, arr, 2, false);
}

void set_pin(uint8_t address, uint8_t reg, uint8_t value)  // Sets value to register corresponding to register address, on address addres
{
    uint8_t arr[2] = {reg, value}; // write to output

    i2c_write_blocking(i2c_default, address, arr, 2, false);
}

uint8_t read_pin(uint8_t address, uint8_t reg)
{
    uint8_t arr[1] = {reg}; // read from input gpio
    uint8_t result;

    i2c_write_blocking(i2c_default, address, arr, 1, true); // write that you'd like to read
    i2c_read_blocking(i2c_default, address, &result, 1, false);  // read into result

    return result;


}

void draw_letter(int x_base, int y_base, char letter)
{
    char index = (letter) - 0x20;
    for(int x_offset = 0; x_offset < 5; x_offset++)
    {
        char col = ASCII[index][x_offset];
        for(int y_offset = 0; y_offset < 8; y_offset++) 
        {
            char pixel = (col >> y_offset) & 0x01;

            ssd1306_drawPixel(x_base + x_offset, y_base + y_offset, pixel);
        }
    }
}

void draw_message(int x_base, int y_base, char* message)
{
    int index = 0;
    while(message[index] != 0)
    {
        draw_letter(x_base, y_base, message[index]);
        x_base += 5;
        index++;
    }
}


int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    ssd1306_setup();
    MCP_setup(ADDR);

    int delta = 0;

    while (true) {
        unsigned int start = to_us_since_boot(get_absolute_time());  

        uint16_t result = adc_read();
        float volts = ((result * 3.3/(1 << 12)));

        char message[50];
        sprintf(message, "ADC0 reading =  %lf", volts);
        draw_message(0, 0, message);

        sprintf(message, "FPS = %.2lf", (1000000.0)/delta);
        draw_message(70, 24, message);
        ssd1306_update();


        delta = to_us_since_boot(get_absolute_time()) - start;
    }
}
