#ifndef HASH_H
#define HASH_H
#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>                                          // JSON parsing library
#include "/home/misha/Downloads/apr-1.7.4/include/apr_hash.h" // Apache Portable Runtime library for HashTable
#include "errors.h"
#include "common.h"
#include "tlv.h"

extern apr_pool_t *pool;
extern apr_hash_t *hash;
extern apr_pool_t *reverse_pool;
extern apr_hash_t *reverse_hash;

extern int hash_key;

void hash_print();
int hash_init();
void hash_add(const char *key, int value);
void hash_destroy();
BOOL hash_key_present(char *key);
BOOL hash_load_tlv(const char *filename, apr_pool_t *pool, apr_hash_t *hash);
BYTE hash_save_tlv(const char *filename, apr_pool_t *pool, apr_hash_t *hash);

#endif