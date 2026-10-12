#include "unity.h"
#include "Mockgpio.h"
#include "Mocktimer.h"
#include "raptor/push_button.h"
#include "gpio_callback_helper.h"
#include "spy.h"
#include <stdbool.h>
#include "raptor/device.h"

extern Device* push_button_from_config(char *config_str);

void test_pin_goes_low(void)
{
    char config[] = "IPB;2Du;50;0;0";

    char* config_pin = &config[4];
    hal_gpio_pin_t expected_pin = 99;
    hal_gpio_parse_pin_ExpectAndReturn(config_pin, expected_pin);

    char* config_pin_pull = &config[6];
    hal_gpio_pull_t expected_pull = HAL_GPIO_PULL_UP;
    hal_gpio_parse_pull_ExpectAndReturn(config_pin_pull, expected_pull);
  
    Device* push_button = push_button_from_config(config);
    
    push_button->ops->init(push_button);
}
  