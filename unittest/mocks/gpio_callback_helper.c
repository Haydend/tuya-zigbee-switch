#include "Mockgpio.h"
#include "gpio_callback_helper.h"

gpio_callback_t gpio_callbacks[3];
hal_gpio_pin_t gpio_pins[3];
void *pin_args[3];

void trigger_pin_change(hal_gpio_pin_t pin)
{
  for (int i = 0; i < 3; i++) {
    if (gpio_pins[i] == pin && gpio_callbacks[i] != NULL) {
      gpio_callbacks[i](pin, pin_args[i]);
    }
  }
}

void captured_hal_gpio_callback(hal_gpio_pin_t gpio_pin, gpio_callback_t callback, void *arg, int cmock_num_calls)
{
  gpio_pins[cmock_num_calls] = gpio_pin;
  gpio_callbacks[cmock_num_calls] = callback;
  pin_args[cmock_num_calls] = arg;
}