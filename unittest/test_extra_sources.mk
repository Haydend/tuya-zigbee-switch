# Per-test extra source files beyond SOURCES.
# Only needed when a test requires additional mocks or the module under test.
# Key format: EXTRA_SRC_<test_filename_without_extension>
#
# Tests not listed here compile with just SOURCES (the common set).

EXTRA_SRC_test_encoder_cluster = ./mocks/Mockstep_command_handler.c $(SRC_DIR)/zigbee/encoder_cluster.c
EXTRA_SRC_test_step_command_handler = $(SRC_DIR)/zigbee/step_command_handler.c
EXTRA_SRC_test_push_button = $(SRC_DIR)/raptor/push_button.c
EXTRA_SRC_test_on_off_client_cluster = $(SRC_DIR)/raptor/on_off_client_cluster.c