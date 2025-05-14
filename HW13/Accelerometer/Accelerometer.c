#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

#define ADDR 0b1101000

#define accel_scale 0.000061
#define gyro_scale 0.007630 
#define temp_scale (1.0/340.00)
#define temp_offset 36.53

// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75

#define X_CENTER 63
#define Y_CENTER 15

int16_t data_values[7];

void accelerometer_setup() {

    uint8_t arr[2] = {0x6b, 0x00}; // Set register
    i2c_write_blocking(i2c_default, ADDR, arr, 2, true);

    uint8_t arr1[2] = {0x1c, 0x00}; // Set register
    i2c_write_blocking(i2c_default, ADDR, arr1, 2, true);

    uint8_t arr2[2] = {0x1b, 0b00011000}; // Set register
    i2c_write_blocking(i2c_default, ADDR, arr2, 2, true);

}


int16_t reconstruct_bytes(uint8_t data_high, uint8_t data_low) {

    return (int16_t) ((data_high << 8) | data_low);
}


void read_data() {

    uint8_t arr[1] = {ACCEL_XOUT_H}; // Read
    uint8_t result[14];

    i2c_write_blocking(i2c_default, ADDR, arr, 1, true);
    i2c_read_blocking(i2c_default, ADDR, result, 14, false);

    for(int i = 0; i < 7; i++)
    {
        data_values[i] = reconstruct_bytes(result[2*i], result[2*i+1]);
    }
    
}

void draw_x_line(int x_val) {

    int x_base;
    int increment = 0;
    

    if (x_val < 0)
    {
        increment = 1;
        x_base = X_CENTER+1;
        x_val = -x_val;

    } else {
        increment = -1;
        x_base = X_CENTER;
        
    }

    for(int i = 0; i < x_val && i < 64; i++)
    {
        ssd1306_drawPixel(x_base + i*increment, Y_CENTER, 1);
        ssd1306_drawPixel(x_base + i*increment, Y_CENTER + 1, 1);

    }
}

void draw_y_line(int y_val) {

    int y_base;
    int increment = 0;
    

    if (y_val < 0)
    {
        increment = -1;
        y_base = Y_CENTER;
        y_val = -y_val;

    } else {
        increment = 1;
        y_base = Y_CENTER+1;
        
    }

    for(int i = 0; i < y_val && i < 16; i++)
    {
        ssd1306_drawPixel(X_CENTER, y_base + i*increment, 1);
        ssd1306_drawPixel(X_CENTER + 1, y_base + i*increment, 1);

    }
}

int main()
{
    stdio_init_all();

    // while (!stdio_usb_connected()) {
    //     sleep_ms(100);
    // }

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    accelerometer_setup();
    ssd1306_setup();

    // See if things working:
    uint8_t arr[1] = {0x75};
    uint8_t result;

    i2c_write_blocking(i2c_default, ADDR, arr, 1, true);
    i2c_read_blocking(i2c_default, ADDR, &result, 1, false);
    
    if(result != 0x68)
    {
        printf("I2C not working\n");
    } else {
        printf("I2C working\n");
    }

    while (true) {

        read_data();
        ssd1306_clear();

        int x = (data_values[0] * accel_scale) * 64;
        int y = (data_values[1] * accel_scale) * 16;

        draw_x_line(x);
        draw_y_line(y);

        ssd1306_update();
        

        // printf("X_accel: %d\n", data_values[0]);
        // printf("Y_accel: %d\n", data_values[1]);
        // printf("Z_accel: %d\n", data_values[2]);

        // printf("Temp: %lf\n", (data_values[3]*temp_scale + temp_offset));

        // printf("X_gyro: %d\n", data_values[4]);
        // printf("Y_gyro: %d\n", data_values[5]);
        // printf("Z_gyro: %d\n", data_values[6]);

        

        sleep_ms(10);
    }
}
