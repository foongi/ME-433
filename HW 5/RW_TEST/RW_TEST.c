#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   13
#define PIN_SCK  18
#define PIN_MOSI 19

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


void spi_ram_init() {

    uint8_t init[2] = {0b00000001, 0b01000000}; // set mode and initialize to sequential mode

    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, init, 2);
    cs_deselect(PIN_CS);
}


void read() {

    uint8_t start_read[3] = {0b00000011, 0b00000000,  0b00000000}; // Ask for read and give address for data (starts at address)

    uint8_t recieve;

    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, start_read, 3);
    spi_read_blocking(SPI_PORT, 0, &recieve, 1);
    cs_deselect(PIN_CS);

    printf("%d\n", recieve);

}

void write() {

    uint8_t start_write[4] = {0b00000010, 0b00000000, 0b00000000}; // Ask for write and give address for data (starts at 0)

    uint8_t number[1] = {0b00001000};

    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, start_write, 3);
    spi_write_blocking(SPI_PORT, number, 1);
    cs_deselect(PIN_CS);

}

int main()
{
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    spi_ram_init();
    write();

    while (true) {
        read();

        sleep_ms(1000);
    }
}
