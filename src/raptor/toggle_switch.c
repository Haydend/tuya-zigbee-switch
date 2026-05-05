#include "toggle_switch.h"
#include "hal/printf_selector.h"
#include "hal/tasks.h"
#include "hal/timer.h"

static void _gpio_callback(hal_gpio_pin_t pin, toggle_switch_t *toggle_switch);

void toggle_switch_init(toggle_switch_t *toggle_switch) {
    toggle_switch->pressed_at_ms = 0;
    toggle_switch->released_at_ms = 0;

    hal_gpio_callback(toggle_switch->pin, (gpio_callback_t)_gpio_callback, toggle_switch);
}

static void _gpio_callback(hal_gpio_pin_t pin, toggle_switch_t *toggle_switch) {
    uint8_t state = hal_gpio_read(pin);

    if ( state == 0 ) {
        toggle_switch->pressed_at_ms = hal_millis();

        if(toggle_switch->on_press != NULL) {
            toggle_switch->on_press(toggle_switch->on_press_callback_param);
        }

    } else {
        toggle_switch->released_at_ms = hal_millis();

        if(toggle_switch->on_release != NULL) {
            uint32_t hold_time = toggle_switch->released_at_ms - toggle_switch->pressed_at_ms;
            toggle_switch->on_release(toggle_switch->on_release_callback_param, hold_time);
        }
    }

}
