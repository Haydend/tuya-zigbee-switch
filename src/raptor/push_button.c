#include "push_button.h"
#include "hal/printf_selector.h"
#include "hal/tasks.h"
#include "hal/timer.h"
#include "hal/gpio.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

static void _gpio_callback(hal_gpio_pin_t pin, push_button_t *push_button);

void push_button_init(push_button_t *push_button) {
    push_button->pressed_at_ms = 0;
    push_button->released_at_ms = 0;
    
    hal_gpio_callback(push_button->pin, (gpio_callback_t)_gpio_callback, push_button);
}

char *seek_until(char *cursor, char needle) {
    while (*cursor != needle && *cursor != '\0') {
        cursor++;
    }
    return(cursor);
}

char *extract_next_entry(char **cursor) {
    char *end = seek_until(*cursor, ';');

    *end = '\0';
    char *res = *cursor;
    *cursor = end + 1;
    return(res);
}

push_button_t *push_button_init_from_config(char* config) {
    char *result = extract_next_entry(&config);

    if (strcmp(result, "IPB") != 0) {
        printf("Config string did not start with expected IPB, actual %s\r\n", result);
        return NULL;
    }

    // Parse Pin 
    char *pin_config = extract_next_entry(&config);
    hal_gpio_pin_t pin = hal_gpio_parse_pin(pin_config);
    hal_gpio_pull_t pull = hal_gpio_parse_pull(pin_config + 2);

    push_button_t *push_button = malloc(sizeof(push_button_t));
    push_button->pin = pin;
    push_button->pin_pull = pull;

    return push_button;
}

static bool _is_pin_pulled_up(push_button_t *push_button) {
    return push_button->pin_pull == HAL_GPIO_PULL_UP || push_button->pin_pull == HAL_GPIO_PULL_UP_1M;
}

static void _gpio_callback(hal_gpio_pin_t pin, push_button_t *push_button) {
    uint8_t state = hal_gpio_read(pin);

    if ( state == !_is_pin_pulled_up(push_button) ) {
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
