#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

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

uint8_t readPin(uint8_t address, uint8_t reg)
{
    uint8_t arr[1] = {reg}; // read from input gpio
    uint8_t result;

    i2c_write_blocking(i2c_default, address, arr, 1, true); // write that you'd like to read
    i2c_read_blocking(i2c_default, address, &result, 1, false);  // read into result

    return result;

}


int main()
{
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    printf("Hello?\n");
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    MCP_setup(ADDR);

    while (true) {
        printf("Hello, world!\n");
        set_pin(ADDR, 0x0a, 0x80);
        sleep_ms(1000);

        set_pin(ADDR, 0x0a, 0x00);
        sleep_ms(1000);

    }
}
