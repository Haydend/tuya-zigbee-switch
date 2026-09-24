#ifndef _TOGGLE_SWITCH_H_
#define _TOGGLE_SWITCH_H_

#include "hal/gpio.h"
#include <stdbool.h>

typedef void (*ev_button_callback_t)(void *);
typedef void (*toggle_switch_on_release_callback_t)(void *, uint32_t hold_time);

typedef struct {
    hal_gpio_pin_t                      pin;
    bool                                pullUp; // Is pin pulled up

    ev_button_callback_t                on_press;
    void *                              on_press_callback_param;
    uint32_t                            pressed_at_ms;
    
    toggle_switch_on_release_callback_t on_release;
    void *                              on_release_callback_param;
    uint32_t                            released_at_ms;

} toggle_switch_t;

void toggle_switch_init(toggle_switch_t *toggle_switch, bool pullUp);

#endif // _TOGGLE_SWITCH_H_