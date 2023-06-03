#ifndef TLV_H
#define TLV_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"
#include "errors.h"
#include "jansson.h"
#include <stdint.h>

#define TLV_TOKEN_LINE 0x4c
#define TLV_TOKEN_INT 0x49
#define TLV_TOKEN_BOOL 0x42
#define TLV_TOKEN_STRING 0x53
#define TLV_TOKEN REAL 0x52

#define TLV_MIN_FILE_SIZE 4

static char tlv_file_name[MAX_FILE_LENGTH];
static FILE *tlv_file;

unsigned char *encode_tlv(uint8_t tag, uint16_t key, const void *data, uint16_t dataLength, uint16_t *encodedLength);
BOOL tlv_init_file(const char *file);
BOOL tlv_write_int(uint16_t key, int value);
BOOL tlv_write_string(uint16_t key,const char *value);
BOOL tlv_write_bool(uint16_t key, BOOL value);
BOOL tlv_write_start();
BOOL tlv_finilize();

int tlv_read_file();
long tlv_file_size(const char* filename);
#endif