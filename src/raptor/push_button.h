#ifndef _PUSH_BUTTON_H_
#define _PUSH_BUTTON_H_

#include "hal/gpio.h"
#include <stdbool.h>

typedef void (*push_button_on_press_callback_t)(void *);
typedef void (*push_button_on_release_callback_t)(void *, uint32_t hold_time);

typedef struct {
    hal_gpio_pin_t                      pin;
    hal_gpio_pull_t                     pin_pull;

    push_button_on_press_callback_t     on_press;
    void *                              on_press_callback_param;
    uint32_t                            pressed_at_ms;

    push_button_on_release_callback_t   on_release;
    void *                              on_release_callback_param;
    uint32_t                            released_at_ms;

} push_button_t;

void push_button_init(push_button_t *push_button);

push_button_t *push_button_init_from_config(char* config);

#endif // _PUSH_BUTTON_H_
