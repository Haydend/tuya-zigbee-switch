#ifndef _ON_OFF_CLIENT_CLUSTER_H_
#define _ON_OFF_CLIENT_CLUSTER_H_

#include "hal/zigbee.h"

typedef struct {
    uint8_t                switch_idx;
    uint8_t                endpoint;
} on_off_client_cluster;

void on_off_client_cluster_add_to_endpoint(on_off_client_cluster *cluster, hal_zigbee_endpoint *endpoint);
void send_toggle_on_off_command(on_off_client_cluster *cluster);

#endif
