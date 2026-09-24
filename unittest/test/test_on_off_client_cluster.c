#include "unity.h"
#include "raptor/on_off_client_cluster.h"
#include "zigbee/consts.h"
#include "Mockzigbee.h"
#include "spy.h"

typedef struct { hal_zigbee_cmd cmd; } send_cmd_args_t;

DECLARE_SPY(send_cmd_spy, send_cmd_args_t);

hal_zigbee_status_t captured_send_cmd_to_bindings(const hal_zigbee_cmd *cmd, int cmock_num_calls)
{
  SPY_RECORD(send_cmd_spy, ((send_cmd_args_t){ .cmd = *cmd }));
  return HAL_ZIGBEE_OK;
}

hal_zigbee_endpoint mock_endpoint = {};
hal_zigbee_cluster endpoint_clusters[10];
on_off_client_cluster cluster = {};

void setUp(void)
{
  // Put a space between tests for readability
  printf("\r\n");

  SPY_RESET(send_cmd_spy);

  // Setup endpoint, cluster and encoder 
  mock_endpoint.endpoint = 1;
  hal_zigbee_cluster endpoint_clusters[10];
  mock_endpoint.clusters = endpoint_clusters;
  mock_endpoint.cluster_count = 0;
  on_off_client_cluster_add_to_endpoint(&cluster, &mock_endpoint);
}

void tearDown(void)
{
}

void test_send_toggle_on_off_command(void) 
{
    // Always report the zigbee status as connected
    hal_zigbee_get_network_status_ExpectAndReturn(HAL_ZIGBEE_NETWORK_JOINED);

    // Capture zigbee commands sent
    hal_zigbee_send_cmd_to_bindings_Stub(captured_send_cmd_to_bindings);

    send_toggle_on_off_command(&cluster);

    // Check one command was sent 
    TEST_ASSERT_EQUAL_MESSAGE(1, send_cmd_spy.calls, "Unexpected number of commands sent");

    // Check command was toggle on/off
    TEST_ASSERT_EQUAL_MESSAGE(mock_endpoint.endpoint, send_cmd_spy.history[0].cmd.endpoint, "Endpoint is wrong");  
    TEST_ASSERT_EQUAL(ZCL_HA_PROFILE, send_cmd_spy.history[0].cmd.profile_id);  
    TEST_ASSERT_EQUAL(ZCL_CLUSTER_ON_OFF, send_cmd_spy.history[0].cmd.cluster_id);  
    TEST_ASSERT_EQUAL(ZCL_CMD_ONOFF_TOGGLE, send_cmd_spy.history[0].cmd.command_id);
    TEST_ASSERT_EQUAL(HAL_ZIGBEE_DIR_CLIENT_TO_SERVER, send_cmd_spy.history[0].cmd.direction);
    TEST_ASSERT_EQUAL(0, send_cmd_spy.history[0].cmd.payload_len);
}