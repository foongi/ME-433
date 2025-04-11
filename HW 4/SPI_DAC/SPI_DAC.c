#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>


// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
// #define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

static float sine[50];
static float triangle[100];


static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}

void writeDAC(int channel, float duty_cycle) {

    uint bits = (int) (duty_cycle * 1023.0);
    
    uint8_t frontbits = ((0b1 & channel) << 7) | (0b111 << 4) | ((0b1111000000 & bits) >> 6);
    uint8_t backbits = ((0b111111 & bits) << 2);


    uint8_t data[2];
    int len = 2;
    data[0] = frontbits;
    data[1] = backbits;

    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, len); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS);

}

void generateWaveforms()
{
    float radians = 0;
    for(int i = 0; i < 50; i++)
    {
        // 2pi radians in a full sine cycle. 50 points in sine cycle
        // 0.1256637 is 1/50 of 2pi

        sine[i] = sin(radians) * 0.5  + 0.5; // augment so from 0 to 1 duty cycle
        radians += 0.1256637;
    }

    // Create triangle duty cycle with 50 samples for up and down each
    // should go from 0 to 1 to 0. 1/50 = 0.02

    float triangle_duty_cycle = 0;
    for(int i = 0; i < 50; i++)
    {
        triangle[i] = triangle_duty_cycle;
        triangle_duty_cycle += 0.02;
    }

    for(int i = 50; i < 100; i++)
    {
        triangle[i] = triangle_duty_cycle;
        triangle_duty_cycle -= 0.02;
    }
}

int main()
{
    stdio_init_all();
    generateWaveforms();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000); //*1000);
    // gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    while (true) {
        printf("Hello, world!\n");

        for(int i = 0; i < 100; i++) // one of these per second
        {
            

            writeDAC(0, sine[(i % 50)]); // Use modulo to get the right sine value
            writeDAC(1, triangle[i]); // Channel 0 for sine, 1 for triangle

            sleep_ms(10);
        }
    }

    
}
