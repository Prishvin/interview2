#ifndef HASH_H
#define HASH_H
#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>                                          // JSON parsing library
#include <apr-1.0/apr_hash.h>
#include <apr-1.0/apr_strings.h>
#include <apr-1.0/apr_pools.h>
#include <apr-1.0/apr_strings.h> // Apache Portable Runtime library for HashTable
#include "errors.h"
#include "../include/common.h"
#include "../include/tlv.h"

extern apr_pool_t *pool;
extern apr_hash_t *hash;
extern apr_pool_t *reverse_pool;
extern apr_hash_t *reverse_hash;

extern int hash_key;

int hash_get_key(const char *key);
void hash_print();
int hash_init();
void hash_add(const char *key, int value);
void hash_destroy();
BOOL hash_key_present(char *key);
BOOL hash_load_tlv(const char *filename, apr_pool_t *pool, apr_hash_t *hash);
BYTE hash_save_tlv(const char *filename, apr_pool_t *pool, apr_hash_t *hash);
size_t hash_count();

apr_hash_t *hash_swap();
void hash_swap_init();
const char* hash_get_value(int key);

#endif