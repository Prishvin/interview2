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
#include "common.h"
#include "tlv.h"

extern _Thread_local apr_pool_t *pool;
extern _Thread_local apr_hash_t *hash;
extern _Thread_local apr_pool_t *reverse_pool;
extern _Thread_local apr_hash_t *reverse_hash;
extern _Thread_local BYTE thread_id;
extern _Thread_local int hash_key;

void hash_print();
int hash_init(int key);
void hash_add(const char *key, int value);
void hash_destroy();
BOOL hash_key_present(char *key);
BOOL hash_load_tlv(const char *filename, apr_pool_t *pool, apr_hash_t *hash);
BYTE hash_save_tlv(const char *filename, apr_pool_t *pool, apr_hash_t *hash);

#endif