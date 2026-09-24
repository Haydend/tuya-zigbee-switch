#include "push_button.h"
#include "hal/printf_selector.h"
#include "hal/tasks.h"
#include "hal/timer.h"
#include <stdbool.h>

static void _gpio_callback(hal_gpio_pin_t pin, push_button_t *push_button);

void push_button_init(push_button_t *push_button, bool pullUp) {
    push_button->pressed_at_ms = 0;
    push_button->released_at_ms = 0;
    push_button->pullUp = pullUp;

    hal_gpio_callback(push_button->pin, (gpio_callback_t)_gpio_callback, push_button);
}

static void _gpio_callback(hal_gpio_pin_t pin, push_button_t *push_button) {
    uint8_t state = hal_gpio_read(pin);

    if ( state == !push_button->pullUp ) {
        push_button->pressed_at_ms = hal_millis();

        if(push_button->on_press != NULL) {
            push_button->on_press(push_button->on_press_callback_param);
        }

    } else {
        push_button->released_at_ms = hal_millis();

        if(push_button->on_release != NULL) {
            uint32_t hold_time = push_button->released_at_ms - push_button->pressed_at_ms;
            push_button->on_release(push_button->on_release_callback_param, hold_time);
        }
    }

}
