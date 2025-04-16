#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>


// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16 // Input pin
#define PIN_CS1  17 // CS pin for DAC
#define PIN_CS2  13 // CS pin for RAM
#define PIN_SCK  18
#define PIN_MOSI 19 // Output to devices

#define START_ADDR 0x0000



// Instead of using the old arrays in on-chip memory, we are going to make an array in external memory
// static float sine[50];
// static float triangle[100];

union FloatInt {
    float f;
    uint32_t i;
};

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

    cs_select(PIN_CS1);
    spi_write_blocking(SPI_PORT, data, len); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS1);

}

float readFloat(uint16_t address)
{
    union FloatInt num;

    uint8_t start_read[3] = {0b00000011, ((address >> 8) & 0xFF),  (address & 0xFF)}; // Ask for read and give address for data (starts at address)

    uint8_t recieve[4];

    cs_select(PIN_CS2);
    spi_write_blocking(SPI_PORT, start_read, 3);
    spi_read_blocking(SPI_PORT, 0, recieve, 4);
    cs_deselect(PIN_CS2);

    num.i = (recieve[0] << 24) + (recieve[1] << 16) + (recieve[2] << 8) + (recieve[3]);


    return num.f;

}


void writeFloat(float *values, int len) {

    union FloatInt num;

    uint8_t start_write[3] = {0b00000010, ((START_ADDR >> 8) & 0xFF), (START_ADDR & 0xFF)}; // Ask for write and give address for data (starts at 0)

    uint8_t broken_float[4];

    cs_select(PIN_CS2);
    spi_write_blocking(SPI_PORT, start_write, 3);

    for(int i = 0; i < len; i++) {
        
        num.f = values[i];
        

        broken_float[0] = (num.i>>24) & 0xFF; // leftmost bits of each float in earlier addresses
        broken_float[1] = (num.i>>16) & 0xFF;
        broken_float[2] = (num.i>>8) & 0xFF;
        broken_float[3] = (num.i) & 0xFF;
        
        spi_write_blocking(SPI_PORT, broken_float, 4); // Loop through every float and write to chip
    }
    
    cs_deselect(PIN_CS2); // Then deselect pin


}

void generateWaveforms()
{

    float sine[1000];

    float radians = 0;
    for(int i = 0; i < 1000; i++)
    {
        // 2pi radians in a full sine cycle. 100 points in sine cycle
        // 0.00628318530718 is 1/1000 of 2pi

        sine[i] = sin(radians) * 0.5  + 0.5; // augment so from 0 to 1 duty cycle (0 to 3.3V)
        radians += 0.00628318530718;

       
    }

    writeFloat(sine, 1000); //pass array of floats to writing function and length = 1000

}

void spi_ram_init() {

    uint8_t init[2] = {0b00000001, 0b01000000}; // set mode and initialize to sequential mode

    cs_select(PIN_CS2);
    spi_write_blocking(SPI_PORT, init, 2);
    cs_deselect(PIN_CS2);
}

int main()
{
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    
    printf("Start!\n");

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS1,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_CS2,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS1, GPIO_OUT);
    gpio_set_dir(PIN_CS2, GPIO_OUT);
    gpio_put(PIN_CS1, 1);
    gpio_put(PIN_CS2, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    // Gpio-dependent initializations
    spi_ram_init();

    printf("Generating and transmitting waveform.\n");
    generateWaveforms();

    printf("Done transmitting waveform\n");

    while (true) {

        int address = START_ADDR;

        for(int i = 0; i < 1000; i++) // one of these per second
        {
            float duty_cycle = readFloat(address);
            writeDAC(0, duty_cycle); // read float and give it to DAC
            printf("Duty cycle (out of 1): %lf\n", duty_cycle);

            address += 4;
            sleep_ms(1);
        }
    }
}
