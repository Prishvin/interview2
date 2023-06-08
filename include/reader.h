#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include "../include/hash.h"
#include "../include/common.h"
#include "../include/tlv.h"
#include <assert.h>

#define MAX_LINE_LENGTH 2048

void iterate_json_object(json_t *json);
void read_json_part(void* arg);
int read_json_file(const char *input_file, const char *output_file, const char *dic_file, size_t *ntokens);