#include "on_off_client_cluster.h"

#include "hal/printf_selector.h"
#include "zigbee/zigbee_commands.h"
#include "zigbee/consts.h"

void on_off_client_cluster_add_to_endpoint(on_off_client_cluster *cluster, hal_zigbee_endpoint *endpoint) {
    
    cluster->endpoint = endpoint->endpoint;

    endpoint->clusters[endpoint->cluster_count].cluster_id      = ZCL_CLUSTER_ON_OFF;
    endpoint->clusters[endpoint->cluster_count].attribute_count = 0;
    endpoint->clusters[endpoint->cluster_count].attributes      = NULL;
    endpoint->clusters[endpoint->cluster_count].is_server       = 0;
    endpoint->cluster_count++;  
}

void on_off_client_cluster_send_toggle_on_off_command(on_off_client_cluster *cluster) {
    if (hal_zigbee_get_network_status() != HAL_ZIGBEE_NETWORK_JOINED) {
        return;
    }

    printf("Sending Toggle On Off Command\r\n");

    hal_zigbee_cmd c = build_onoff_cmd(cluster->endpoint, ZCL_CMD_ONOFF_TOGGLE);
    hal_zigbee_send_cmd_to_bindings(&c);
}


