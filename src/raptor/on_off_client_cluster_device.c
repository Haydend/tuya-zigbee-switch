#include "device.h"
#include "hal/printf_selector.h"
#include <stdlib.h>

typedef struct {
    char* name
} OnOffClientClusterData;

static void send_toggle_command(Device *self) {
    OnOffClientClusterData *data = (OnOffClientClusterData *)self->private_data;

    printf("Sending Toggle Command\n");
}

static void init(Device *self) {}
static void register_callback(Device *self, int i, void *cb, void *cb_param) {}

static void* get_callback(Device *self, int i) {
    if(i == 0) {
        return send_toggle_command;
    }

    return NULL;
}

const DeviceOperations device_ops = {
    .init = init,
    .register_callback = register_callback,
    .get_callback = get_callback
};

Device* on_off_client_cluster_from_config(char *config) {

    // 1. Allocate memory for the generic Device wrapper
    Device *dev = malloc(sizeof(Device));
    if (!dev) return NULL;

    // 2. Allocate memory for this specific button's private data
    OnOffClientClusterData *data = malloc(sizeof(OnOffClientClusterData));
    if (!data) {
        free(dev);
        return NULL;
    }

    // 3. Assign the custom properties
    data->name = "Cluster";
    // data->pin = pin;
    // data->pin_pull = pull;

    // 4. Bind them together
    dev->ops = &device_ops;
    dev->private_data = data;

    return dev;
}