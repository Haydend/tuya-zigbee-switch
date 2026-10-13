#ifndef PUSH_BUTTON_DEVICE_H
#define PUSH_BUTTON_DEVICE_H

#include "hal/gpio.h"
#include "device.h"

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
} PushButtonData;

Device* push_button_from_config(char *config);

#endif