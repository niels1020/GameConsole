#include "main.h"

void ButtonPressed(uint button, bool state){
    INPUT(button, state);
}

int main()
{
    stdio_init_all();
    sleep_ms(1000);
    printf("initializing\n");

    struct st7789_config config = {
        .spi = spi0,    // Use SPI0 (adjust if using a different SPI port)
        .gpio_din = 19, // Data input (MOSI) pin (adjust as needed)
        .gpio_clk = 18, // Clock pin (SCK) (adjust as needed)
        .gpio_cs = -1,  // Chip select pin (CS)
        .gpio_dc = 20,  // Data/Command pin (DC)
        .gpio_rst = 15, // Reset pin (RST)
        .gpio_bl = 10   // Backlight pin (optional)
    };
    
    st7789_init(&config, SCREEN_HEIGHT, SCREEN_WIDTH);
    
    st7789_fill(Color::COLOR_GREEN);

    InputInit();

    printf("initialized\n");

    LAUNCH();
}
