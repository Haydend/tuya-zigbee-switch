#ifndef PARSER_H
#define PARSER_H

#include "device.h"
#include <stddef.h>

Device* device_parse_config(char *config_str);
size_t device_parse_config_list(char **config_strings, size_t count, Device **out_devices);

#endif
