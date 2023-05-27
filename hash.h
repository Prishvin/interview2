#ifndef HASH_H
#define HASH_H
#include <stdio.h>
#include <stdlib.h>
#include <jansson.h> // JSON parsing library
#include "/home/misha/Downloads/apr-1.7.4/include/apr_hash.h" // Apache Portable Runtime library for HashTable
#include "errors.h"
#include "common.h"



extern apr_pool_t *pool;
extern apr_hash_t *hash;

extern  int hash_key;

void hash_print();
int hash_init();
void hash_add(const char* key, int value);
void hash_destroy();
BOOL hash_key_present(char* key);


#endif