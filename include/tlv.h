#ifndef TLV_H
#define TLV_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../include/common.h"
#include "errors.h"
#include "jansson.h"
#include <stdint.h>

#define TLV_TOKEN_LINE 0x4c
#define TLV_TOKEN_INT 0x49
#define TLV_TOKEN_BOOL 0x42
#define TLV_TOKEN_STRING 0x53
#define TLV_TOKEN REAL 0x52

#define TLV_MIN_FILE_SIZE 4

extern char tlv_file_name[MAX_FILE_LENGTH];
extern FILE *tlv_output_file;

unsigned char *encode_tlv(uint8_t tag, uint16_t key, const void *data, uint16_t dataLength, uint16_t *encodedLength);
BOOL tlv_init_file(const char *file_name, FILE** file);
BOOL tlv_write_int(uint16_t key, int value, FILE* file);
BOOL tlv_write_string(uint16_t key, const char *value, FILE* file);
BOOL tlv_write_bool(uint16_t key, BOOL value, FILE* file);
BOOL tlv_write_start_line(FILE* file);
BOOL tlv_finilize(FILE* file);

int tlv_read_json(const char *filename, json_t **master_json);
long tlv_file_size(const char *filename);
#endif