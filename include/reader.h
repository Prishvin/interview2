#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include "hash.h"
#include "common.h"
#include "tlv.h"
#include <assert.h>

#define MAX_LINE_LENGTH 2048

void iterate_json_object(json_t *json);
void read_json_part(void* arg);
int read_json_file(const char *file_name);