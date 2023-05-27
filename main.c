#include <stdio.h>
#include <stdlib.h>
#include <jansson.h> // JSON parsing library
#include "/home/misha/Downloads/apr-1.7.4/include/apr_hash.h" // Apache Portable Runtime library for HashTable
#include "pthread.h"


void print_hash(apr_pool_t *pool, apr_hash_t *hash) {
    apr_hash_index_t *hi;

    // Iterate over the keys in the hash table
    for (hi = apr_hash_first(pool, hash); hi; hi = apr_hash_next(hi)) {
        const char *key;
        int *val;

        // Get the current key and value
        apr_hash_this(hi, (const void**)&key, NULL, (void**)&val);

        // Print the key-value pair
        printf("%s: %d\n", key, *val);
    }
}
int main()
{
    apr_pool_t *pool;
    apr_hash_t *hash;

    apr_initialize();
    atexit(apr_terminate);

    apr_pool_create(&pool, NULL);

    hash = apr_hash_make(pool);

    int val1 = 1;
    int val2 = 2;
    int val3 = 3;

    apr_hash_set(hash, "one", APR_HASH_KEY_STRING, &val1);

    apr_hash_set(hash, "one", APR_HASH_KEY_STRING, &val1);
    apr_hash_set(hash, "two", APR_HASH_KEY_STRING, &val2);
    apr_hash_set(hash, "three", APR_HASH_KEY_STRING, &val3);

    print_hash(pool, hash);

    apr_pool_destroy(pool);

    return 0;
}