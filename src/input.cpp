#include "main.h"

void GpioCallback(uint gpio, uint32_t event_mask)
{
    if (event_mask == GPIO_IRQ_EDGE_RISE)
    {
        ButtonPressed(gpio, true);
    }
    else if (event_mask == GPIO_IRQ_EDGE_FALL)
    {
        ButtonPressed(gpio, false);
    }
}

/*
DEBOUNCE
#define DEBOUNCE_DELAY_MS 50
static absolute_time_t last_interrupt_time = 0;

void GpioCallback(uint gpio, uint32_t event_mask) {
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(last_interrupt_time, now) < DEBOUNCE_DELAY_MS * 1000) {
        return; // Ignore bounce
    }
    last_interrupt_time = now;

    printf("GPIO callback. Pin: %d, Event: %d\n", gpio, event_mask);
    ButtonPressed(gpio, event_mask == GPIO_IRQ_EDGE_RISE);
}
*/

void InputInit()
{
    // initialize gpio for pins
    gpio_init(Button1);
    gpio_init(Button2);
    gpio_init(Button3);
    gpio_init(Button4);

    // set pin directions
    gpio_set_dir(Button1, GPIO_IN);
    gpio_set_dir(Button2, GPIO_IN);
    gpio_set_dir(Button3, GPIO_IN);
    gpio_set_dir(Button4, GPIO_IN);

    // pul down pins
    gpio_pull_down(Button1);
    gpio_pull_down(Button2);
    gpio_pull_down(Button3);
    gpio_pull_down(Button4);

    //enable interupts
    irq_set_enabled(IO_IRQ_BANK0, true);

    // set calback
    gpio_set_irq_callback(GpioCallback);

    // enable calbacks
    gpio_set_irq_enabled(Button1, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(Button2, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(Button3, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(Button4, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}