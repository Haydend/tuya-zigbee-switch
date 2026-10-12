#ifndef DEVICE_H
#define DEVICE_H

typedef struct Device Device;

typedef struct {
    void (*init)(Device *self);
    void (*register_callback)(Device *self, int i, void* cb, void* cb_param);
} DeviceOperations;

struct Device {
    const DeviceOperations *ops;
    void *private_data;
};

// Signature that every device's custom parser must match
// It takes the raw config string and returns a constructed Device pointer
typedef Device* (DeviceParserFunc)(char *config_str);

// The core configuration router
Device* device_parse_config(char *config_str);

#endif