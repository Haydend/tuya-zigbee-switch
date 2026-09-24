#include "unity.h"
#include "zigbee/step_command_handler.h"
#include "Mocktimer.h"
#include "Mocktasks.h"
#include "spy.h"

typedef struct { void *arg; int change; uint16_t trans_time; } step_cb_args_t;

DECLARE_SPY(step_spy, step_cb_args_t);

void mock_callback(void * arg, int change, uint16_t trans_time) {
  SPY_RECORD(step_spy, ((step_cb_args_t){ .arg = arg, .change = change, .trans_time = trans_time }));
}

void setUp(void)
{
  // Reset
  SPY_RESET(step_spy);

  // Put a space between tests for readability
  printf("\r\n");
}

void tearDown(void)
{
}

step_command_handler_t step_command_handler = {};

void test_first_call_to_step_up_triggers_callback(void) {
    new_step_command_handler(&step_command_handler);

    int arg = 6;
    step_command_handler_register_callback(&step_command_handler, mock_callback, &arg);

    hal_millis_IgnoreAndReturn(101); // Make time 100ms since start up
    step_command_handler_step_up(&step_command_handler);
    
    // Callback was called once, with the expected arguments
    ASSERT_SPY_CALLED(step_spy, 1);
    TEST_ASSERT_EQUAL(&arg, step_spy.history[0].arg); // Check the pointer we got back is the same as the pointer we sent 
    TEST_ASSERT_EQUAL(13, step_spy.history[0].change); // A single step up, should be 13
    TEST_ASSERT_EQUAL(1, step_spy.history[0].trans_time); 
}

void test_first_call_to_step_down_triggers_callback(void) {
    new_step_command_handler(&step_command_handler);

    int arg = 6;
    step_command_handler_register_callback(&step_command_handler, mock_callback, &arg);

    hal_millis_IgnoreAndReturn(100); // Make time 100ms since start up
    step_command_handler_step_down(&step_command_handler);
    
    // Callback was called once, with the expected arguments
    ASSERT_SPY_CALLED(step_spy, 1);
    TEST_ASSERT_EQUAL(&arg, step_spy.history[0].arg); // Check the pointer we got back is the same as the pointer we sent 
    TEST_ASSERT_EQUAL(-13, step_spy.history[0].change); // A single step down, should be -13
    TEST_ASSERT_EQUAL(1, step_spy.history[0].trans_time); 
}

void test_step_up_calls_very_close_together_are_debounced(void) {
    new_step_command_handler(&step_command_handler);

    int arg = 6;
    step_command_handler_register_callback(&step_command_handler, mock_callback, &arg);

    // Send a first step up command, at 100ms after start up, callback will be triggered
    hal_millis_IgnoreAndReturn(100);
    step_command_handler_step_up(&step_command_handler);
    ASSERT_SPY_CALLED(step_spy, 1);
    TEST_ASSERT_EQUAL(&arg, step_spy.history[0].arg); 
    TEST_ASSERT_EQUAL(13, step_spy.history[0].change);
    TEST_ASSERT_EQUAL(1, step_spy.history[0].trans_time); 

    // Send a second step up command, 10ms after the first, callback will not be triggered. Change will be queued
    hal_millis_IgnoreAndReturn(105);
    step_command_handler_step_up(&step_command_handler);
    ASSERT_SPY_CALLED(step_spy, 1); // Still only called once.

    // Send a third step up command, 100ms after the first, callback will be triggered.
    hal_millis_IgnoreAndReturn(200);
    step_command_handler_step_up(&step_command_handler);
    ASSERT_SPY_CALLED(step_spy, 2); // Has been called a second time
    TEST_ASSERT_EQUAL(&arg, step_spy.history[1].arg); 
    TEST_ASSERT_EQUAL(26, step_spy.history[1].change); // Change includes the queued change from the second step up call.
    TEST_ASSERT_EQUAL(1, step_spy.history[1].trans_time); 
}

void test_step_down_calls_very_close_together_are_debounced(void) {
    new_step_command_handler(&step_command_handler);

    int arg = 6;
    step_command_handler_register_callback(&step_command_handler, mock_callback, &arg);

    // Send a first step down command, at 100ms after start up, callback will be triggered
    hal_millis_IgnoreAndReturn(100);
    step_command_handler_step_down(&step_command_handler);
    ASSERT_SPY_CALLED(step_spy, 1);
    TEST_ASSERT_EQUAL(&arg, step_spy.history[0].arg); 
    TEST_ASSERT_EQUAL(-13, step_spy.history[0].change);
    TEST_ASSERT_EQUAL(1, step_spy.history[0].trans_time); 

    // Send a second step down command, 10ms after the first, callback will not be triggered. Change will be queued
    hal_millis_IgnoreAndReturn(105);
    step_command_handler_step_down(&step_command_handler);
    ASSERT_SPY_CALLED(step_spy, 1); // Still only called once.

    // Send a third step down command, 100ms after the first, callback will be triggered.
    hal_millis_IgnoreAndReturn(200);
    step_command_handler_step_down(&step_command_handler);
    ASSERT_SPY_CALLED(step_spy, 2); // Has been called a second time
    TEST_ASSERT_EQUAL(&arg, step_spy.history[1].arg); 
    TEST_ASSERT_EQUAL(-26, step_spy.history[1].change); // Change includes the queued change from the second step up call.
    TEST_ASSERT_EQUAL(1, step_spy.history[1].trans_time); 
}
