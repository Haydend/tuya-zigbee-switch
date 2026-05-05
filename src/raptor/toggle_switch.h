#ifndef _TOGGLE_SWITCH_H_
#define _TOGGLE_SWITCH_H_

#include "hal/gpio.h"

typedef void (*ev_button_callback_t)(void *);

typedef struct {
    hal_gpio_pin_t                   pin;

    ev_button_callback_t             on_press;
    void *                           on_press_callback_param;
} toggle_switch_t;

void toggle_switch_init(toggle_switch_t *toggle_switch);
void register_on_press_cb(toggle_switch_t *toggle_switch, ev_button_callback_t on_press, void *on_press_callback_param);

#endif // _TOGGLE_SWITCH_H_