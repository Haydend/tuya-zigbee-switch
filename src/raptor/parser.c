#include "parser.h"
#include <stddef.h>
#include <string.h>
#include "push_button_device.h"
#include "on_off_client_cluster_device.h"

typedef struct {
    const char *prefix;
    DeviceParserFunc *parse;
} ParserRegistry;

static const ParserRegistry registry[] = {
    { "IPB", push_button_from_config },
    { "ZCOO",  on_off_client_cluster_from_config  },
    // Adding a new device type? Just insert one line here!
};

#define REGISTRY_SIZE (sizeof(registry) / sizeof(registry[0]))

Device* device_parse_config(char *config_str) {
    if (config_str == NULL) {
        return NULL;
    }

    // Extract the prefix from the config string
    char *prefix_end = strchr(config_str, ';');
    if (prefix_end == NULL) {
        return NULL; // Invalid format
    }

    size_t prefix_length = prefix_end - config_str;
    char prefix[prefix_length + 1];
    strncpy(prefix, config_str, prefix_length);
    prefix[prefix_length] = '\0';

    // Search for the corresponding parser in the registry
    for (size_t i = 0; i < REGISTRY_SIZE; ++i) {
        if (strcmp(prefix, registry[i].prefix) == 0) {
            return registry[i].parse(config_str);
        }
    }

    return NULL; // No matching parser found
}

// Parses an array of configuration strings and populates an array of Devices
size_t device_parse_config_list(char **config_strings, size_t count, Device **out_devices) {
    if (!config_strings || !out_devices || count == 0) return 0;

    size_t successfully_parsed = 0;

    for (size_t i = 0; i < count; i++) {
        out_devices[i] = device_parse_config(config_strings[i]);
        
        if (out_devices[i] != NULL) {
            successfully_parsed++;
        }
    }

    return successfully_parsed;
}