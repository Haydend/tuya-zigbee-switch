#include "unity.h"
#include "raptor/core.h"

void setUp(void)
{
  // Put a space between tests for readability
  printf("\r\n");
}

void tearDown(void)
{
}

void test_core_init(void) 
{
    core_init();
}