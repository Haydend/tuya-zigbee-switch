#include "device.h"
#include "hal/printf_selector.h"
#include "hal/tasks.h"
#include "hal/timer.h"
#include "hal/gpio.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    hal_gpio_pin_t                      pin;
    hal_gpio_pull_t                     pin_pull;
} PushButtonData;

static void init(Device *self) {
    PushButtonData *data = (PushButtonData *)self->private_data;

    printf("Initialising Push Button Device, pin %d\n", data->pin);
}

const DeviceOperations push_button_device_ops = {
    .init = init
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