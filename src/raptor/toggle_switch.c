#include "toggle_switch.h"
#include "hal/printf_selector.h"
#include "hal/tasks.h"

static void _gpio_callback(hal_gpio_pin_t pin, toggle_switch_t *toggle_switch);

void toggle_switch_init(toggle_switch_t *toggle_switch) {
    hal_gpio_callback(toggle_switch->pin, (gpio_callback_t)_gpio_callback, toggle_switch);
}

static void _gpio_callback(hal_gpio_pin_t pin, toggle_switch_t *toggle_switch) {
    printf("Callback Actions\r\n");

    toggle_switch->on_press(toggle_switch->on_press_callback_param);
}