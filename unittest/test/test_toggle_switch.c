#include "unity.h"
#include "Mockgpio.h"
#include "Mocktimer.h"
#include "raptor/toggle_switch.h"
#include "gpio_callback_helper.h"
#include <string.h>

typedef struct {
  int calls;
  void *last_arg;
} press_spy_t;

typedef struct {
  int calls;
  void *last_arg;
  uint32_t last_hold_time;
} release_spy_t;

press_spy_t press_spy;
release_spy_t release_spy;

int on_press_cb_arg = 1;
int on_release_cb_arg = 2;

void on_press(void *arg) {
  press_spy.calls++;
  press_spy.last_arg = arg;
}

void on_release(void *arg, uint32_t hold_time) {
  release_spy.calls++;
  release_spy.last_arg = arg;
  release_spy.last_hold_time = hold_time;
}

void assert_press_called(int expected_calls, void *expected_arg) {
  TEST_ASSERT_EQUAL(expected_calls, press_spy.calls);
  if (expected_calls > 0) {
    TEST_ASSERT_EQUAL_PTR(expected_arg, press_spy.last_arg);
  }
}

void assert_press_not_called(void) {
  TEST_ASSERT_EQUAL(0, press_spy.calls);
}

void assert_release_called(int expected_calls, void *expected_arg, uint32_t expected_hold_time) {
  TEST_ASSERT_EQUAL(expected_calls, release_spy.calls);
  if (expected_calls > 0) {
    TEST_ASSERT_EQUAL_PTR(expected_arg, release_spy.last_arg);
    TEST_ASSERT_EQUAL(expected_hold_time, release_spy.last_hold_time);
  }
}

void assert_release_not_called(void) {
  TEST_ASSERT_EQUAL(0, release_spy.calls);
}

void setUp(void)
{
  // Put a space between tests for readability
  printf("\r\n");

  // Reset
  memset(&press_spy, 0, sizeof(press_spy));
  memset(&release_spy, 0, sizeof(release_spy));
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
  
  assert_press_called(1, &on_press_cb_arg);
  assert_release_not_called();
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
  
  assert_release_called(1, &on_release_cb_arg, 10);
  assert_press_not_called();
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