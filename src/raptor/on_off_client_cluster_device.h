#ifndef ON_OFF_CLIENT_CLUSTER_DEVICE_H
#define ON_OFF_CLIENT_CLUSTER_DEVICE_H

#include "device.h"

typedef struct {
    char* name;
} OnOffClientClusterData;

Device* on_off_client_cluster_from_config(char *config);

#endif