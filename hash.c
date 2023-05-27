#include "hash.h"
#include <apr.h>
#include <apr_strings.h>
#include <apr_pools.h>
#include <apr_strings.h>

int hash_key = 0;
apr_pool_t *pool = NULL;
apr_hash_t *hash = NULL;

void hash_print()
{
    apr_hash_index_t *hi;

    // Iterate over the keys in the hash table
    for (hi = apr_hash_first(pool, hash); hi; hi = apr_hash_next(hi))
    {
        const char *key;
        int *val;

        // Get the current key and value
        apr_hash_this(hi, (const void **)&key, NULL, (void **)&val);

        // Print the key-value pair
        printf("%s: %d\n", key, *val);
    }
}

int hash_init()
{
    hash_key = 0;
    int rc = apr_initialize();
    if (rc != APR_SUCCESS)
    {
        perror("apr init");
        printf("Failed to initialize APR\n");
        return 1;
    }
    apr_pool_create(&pool, NULL);

    hash = apr_hash_make(pool);
    if (hash == NULL)
        return ERROR_HASH_CREATION_FAILED;
    else
    {
        printf("APR initialized \n");
        return ERROR_NONE;
    }
}

void hash_destroy()
{
    apr_pool_destroy(pool);
    apr_terminate();
}

BOOL hash_key_present(char *key)
{
    const char *value = apr_hash_get(hash, key, APR_HASH_KEY_STRING);
    if (value != NULL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

int hash_get_key(char *key)
{
    int result = -1;
    int *value = apr_hash_get(hash, key, APR_HASH_KEY_STRING);
    if (value != NULL)
    {
        result = *value;
    }

    return result;
}

void hash_add(const char *key, int value)
{
    int *pValue = apr_palloc(pool, sizeof(int));
    if (pValue == NULL)
    {
        perror("Hash item allocation error");
    }
    *pValue = value;
    char *pKey = apr_pstrdup(pool, key);
    if (pKey == NULL)
    {
        perror("Key allocation error");
        return;
    }
    apr_hash_set(hash, pKey, APR_HASH_KEY_STRING, pValue);
}