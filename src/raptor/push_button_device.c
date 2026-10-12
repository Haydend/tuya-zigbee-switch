#include "device.h"
#include "hal/printf_selector.h"
#include "hal/tasks.h"
#include "hal/timer.h"
#include "hal/gpio.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

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

static void _gpio_callback(hal_gpio_pin_t pin, Device *self);

static void init(Device *self) {
    PushButtonData *data = (PushButtonData *)self->private_data;

    printf("Initialising Push Button Device, pin %d\n", data->pin);

    data->pressed_at_ms = 0;
    data->released_at_ms = 0;

    hal_gpio_callback(data->pin, (gpio_callback_t)_gpio_callback, self);
}

static bool _is_pin_pulled_up(PushButtonData *data) {
    return data->pin_pull == HAL_GPIO_PULL_UP || data->pin_pull == HAL_GPIO_PULL_UP_1M;
}

static void _gpio_callback(hal_gpio_pin_t pin, Device *self) {
    PushButtonData *data = (PushButtonData *)self->private_data;
    uint8_t state = hal_gpio_read(pin);

    if ( state == !_is_pin_pulled_up(data) ) {
        data->pressed_at_ms = hal_millis();

        if(data->on_press != NULL) {
            data->on_press(data->on_press_callback_param);
        }

    } else {
        data->released_at_ms = hal_millis();

        if(data->on_release != NULL) {
            uint32_t hold_time = data->released_at_ms - data->pressed_at_ms;
            data->on_release(data->on_release_callback_param, hold_time);
        }
    }

}

static void register_callback(Device *self, int i, void *cb, void *cb_param) {
    PushButtonData *data = (PushButtonData *)self->private_data;

    if(i == 0) {
        data->on_press = cb;
        data->on_press_callback_param = cb_param;

    } else if(i == 1) {
        data->on_release = cb;
        data->on_release_callback_param = cb_param;

    } else {
        printf("Unsupported callback index for push button device: %d", i);
    }
}

static void* get_callback(Device *self, int i) {
    return NULL;
}

const DeviceOperations push_button_device_ops = {
    .init = init,
    .register_callback = register_callback,
    .get_callback = get_callback
};

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

Device* push_button_from_config(char *config) {
    
    // ===== Parse Config =====
    char *result = extract_next_entry(&config);

    if (strcmp(result, "IPB") != 0) {
        printf("Config string did not start with expected IPB, actual %s\r\n", result);
        return NULL;
    }

    // Parse Pin 
    char *pin_config = extract_next_entry(&config);
    hal_gpio_pin_t pin = hal_gpio_parse_pin(pin_config);
    hal_gpio_pull_t pull = hal_gpio_parse_pull(pin_config + 2);

    // ===========
    
    // 1. Allocate memory for the generic Device wrapper
    Device *dev = malloc(sizeof(Device));
    if (!dev) return NULL;

    // 2. Allocate memory for this specific button's private data
    PushButtonData *data = malloc(sizeof(PushButtonData));
    if (!data) {
        free(dev);
        return NULL;
    }

    // 3. Assign the custom properties
    data->pin = pin;
    data->pin_pull = pull;

    // 4. Bind them together
    dev->ops = &push_button_device_ops;
    dev->private_data = data;

    return dev;
}