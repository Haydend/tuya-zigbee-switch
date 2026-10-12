#include "unity.h"
#include "Mockgpio.h"
#include "Mocktimer.h"
#include "raptor/push_button.h"
#include "gpio_callback_helper.h"
#include "spy.h"
#include <stdbool.h>
#include "raptor/device.h"

typedef struct { void *arg; } press_args_t;
typedef struct { void *arg; uint32_t hold_time; } release_args_t;

DECLARE_SPY(press_spy, press_args_t);
DECLARE_SPY(release_spy, release_args_t);
int on_press_cb_arg = 1;
int on_release_cb_arg = 2;

void on_press(void *arg) {
  SPY_RECORD(press_spy, ((press_args_t){ .arg = arg }));
}

void on_release(void *arg, uint32_t hold_time) {
  SPY_RECORD(release_spy, ((release_args_t){ .arg = arg, .hold_time = hold_time }));
}

void setUp(void)
{
  // Put a space between tests for readability
  printf("\r\n");

  // Reset
  SPY_RESET(press_spy);
  SPY_RESET(release_spy);

  hal_gpio_callback_StubWithCallback(captured_hal_gpio_callback);
}

extern Device* push_button_from_config(char *config_str);
extern Device* on_off_client_cluster_from_config(char *config);

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

    Device* on_off_client_cluster = on_off_client_cluster_from_config(config);
    on_off_client_cluster->ops->init(on_off_client_cluster);

    void* callback = on_off_client_cluster->ops->get_callback(on_off_client_cluster, 0);
    push_button->ops->register_callback(push_button, 0, callback, on_off_client_cluster);

    // Pin changes to low
    hal_millis_IgnoreAndReturn(10);
    hal_gpio_read_ExpectAndReturn(expected_pin, 0);
    trigger_pin_change(expected_pin);

    // Check on_press callback triggered with expected args
    //ASSERT_SPY_CALLED(press_spy, 1);
    //TEST_ASSERT_EQUAL_PTR(&on_press_cb_arg, press_spy.history[0].arg);
}
  