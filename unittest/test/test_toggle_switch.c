#include "unity.h"
#include "Mockgpio.h"
#include "Mocktimer.h"
#include "raptor/toggle_switch.h"
#include "gpio_callback_helper.h"

int on_press_calls = 0;
void on_press(void)
{
  on_press_calls++;
}

void setUp(void)
{
  // Put a space between tests for readability
  printf("\r\n");

  // Reset
  on_press_calls = 0;
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
  int arg = 1;
  register_on_press_cb(toggle_switch, (ev_button_callback_t)on_press, &arg);
}

void _trigger_pin_change(int callback_cnt, hal_gpio_pin_t pin, uint8_t new_state, uint32_t time_of_change)
{
  // Prep For pin a changing
  hal_millis_IgnoreAndReturn(time_of_change);
  hal_gpio_read_ExpectAndReturn(pin, new_state);

  // Trigger gpio change call back
  trigger_pin_change(callback_cnt);
}

void test_encoder_pin_changing(void)
{
  // Setup Encoder, with all pins high
  toggle_switch_t toggle_switch = {};
  _setup_toggle_switch(&toggle_switch);

  // Pin changes
  //hal_gpio_read_ExpectAndReturn(toggle_switch.pin, 0);
  trigger_pin_change(0);
  
  // no other callbacks triggered 
  TEST_ASSERT_EQUAL(1, on_press_calls);
}
