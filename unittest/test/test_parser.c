#include "unity.h"
#include "raptor/parser.h"
#include "Mockgpio.h"

void test_parser(void) {

    hal_gpio_pin_t expected_pin = 99;
    hal_gpio_parse_pin_ExpectAnyArgsAndReturn(expected_pin);

    hal_gpio_pull_t expected_pull = HAL_GPIO_PULL_UP;
    hal_gpio_parse_pull_ExpectAnyArgsAndReturn(expected_pull);


    char configs[][32] = {
        "IPB;2Du;50;0;0",
        "ZCOO;ClusterConfigData",
    };
    size_t num_configs = sizeof(configs) / sizeof(configs[0]);

    // To pass this into your parser, you build a temporary pointer array:
    char *config_ptrs[num_configs];
    for (size_t i = 0; i < num_configs; i++) {
        config_ptrs[i] = configs[i];
    }


    Device *devices[num_configs];
    size_t valid_count = device_parse_config_list(config_ptrs, num_configs, devices);

    return;
}