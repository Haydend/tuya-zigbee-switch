#include "unity.h"
#include "Mockgpio.h"
#include "Mocktimer.h"
#include "raptor/toggle_switch.h"
#include "gpio_callback_helper.h"
#include "spy.h"

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
}

void tearDown(void)
{
}

void _setup_toggle_switch(toggle_switch_t *toggle_switch)
{
  // Setup Encoder
  toggle_switch->pin = 1;

  hal_gpio_callback_StubWithCallback(captured_hal_gpio_callback);

  toggle_switch_init(toggle_switch);

  toggle_switch->on_press = (ev_button_callback_t)on_press;
  toggle_switch->on_press_callback_param = &on_press_cb_arg;

  toggle_switch->on_release = (toggle_switch_on_release_callback_t)on_release;
  toggle_switch->on_release_callback_param = &on_release_cb_arg;
}

void test_pin_goes_low(void)
{
  toggle_switch_t toggle_switch = {};
  _setup_toggle_switch(&toggle_switch);

  // Pin changes
  hal_millis_IgnoreAndReturn(10);
  hal_gpio_read_ExpectAndReturn(toggle_switch.pin, 0);
  trigger_pin_change(0);
  
  // Check on_press callback triggered with expected args
  ASSERT_SPY_CALLED(press_spy, 1);
  TEST_ASSERT_EQUAL_PTR(&on_press_cb_arg, press_spy.history[0].arg);

  // Check on_release callback not triggered
  ASSERT_SPY_NOT_CALLED(release_spy);
}

void test_pin_goes_low_no_on_press_callback_defined(void)
{
  // Checking logic does not hit seg fault

  toggle_switch_t toggle_switch = {};
  _setup_toggle_switch(&toggle_switch);
  toggle_switch.on_press = NULL;

  // Pin changes
  hal_millis_IgnoreAndReturn(10);
  hal_gpio_read_ExpectAndReturn(toggle_switch.pin, 0);
  trigger_pin_change(0);
  
  // no seg fault
}

void test_pin_goes_high(void)
{
  toggle_switch_t toggle_switch = {};
  _setup_toggle_switch(&toggle_switch);

  // Pin changes
  hal_millis_IgnoreAndReturn(10);
  hal_gpio_read_ExpectAndReturn(toggle_switch.pin, 1);
  trigger_pin_change(0);
  
  // Check on_release callback triggered with expected args
  ASSERT_SPY_CALLED(release_spy, 1);
  TEST_ASSERT_EQUAL_PTR(&on_release_cb_arg, release_spy.history[0].arg);
  TEST_ASSERT_EQUAL(10, release_spy.history[0].hold_time);

  // Check on_press callback not triggered
  ASSERT_SPY_NOT_CALLED(press_spy);
}

void test_pin_goes_high_no_on_release_callback_defined(void)
{
  toggle_switch_t toggle_switch = {};
  _setup_toggle_switch(&toggle_switch);
  toggle_switch.on_release = NULL;

  // Pin changes
  hal_millis_IgnoreAndReturn(10);
  hal_gpio_read_ExpectAndReturn(toggle_switch.pin, 1);
  trigger_pin_change(0);
  
  // No seg fault!
}