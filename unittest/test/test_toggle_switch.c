#include "unity.h"
#include "Mockgpio.h"
#include "Mocktimer.h"
#include "raptor/toggle_switch.h"
#include "gpio_callback_helper.h"

int on_press_cb_arg = 1;
int on_press_calls = 0;
void *on_press_last_arg = NULL;
void on_press(void *arg) {
  on_press_calls++;
  on_press_last_arg = arg;
}

int on_release_cb_arg = 2;
int on_release_calls = 0;
void *on_release_last_arg = NULL;
uint32_t on_release_last_on_time = 0;
void on_release(void *arg, uint32_t on_time) {
  on_release_calls++;
  on_release_last_arg = arg;
  on_release_last_on_time = on_time;
}

void setUp(void)
{
  // Put a space between tests for readability
  printf("\r\n");

  // Reset
  on_press_calls = 0;
  on_release_calls = 0;
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
  
  // on press callback triggered 
  TEST_ASSERT_EQUAL(1, on_press_calls);
  TEST_ASSERT_EQUAL(1, *(int *)on_press_last_arg);

  TEST_ASSERT_EQUAL(0, on_release_calls);
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
  
  // on release callback triggered once
  TEST_ASSERT_EQUAL(1, on_release_calls);
  TEST_ASSERT_EQUAL(2, *(int *)on_release_last_arg);
  TEST_ASSERT_EQUAL(10, on_release_last_on_time);

  //  on press callback not triggered 
  TEST_ASSERT_EQUAL(0, on_press_calls);
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

void test_pin_changes_are_debounced(void) 
{
  

}