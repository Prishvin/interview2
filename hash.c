#include "hash.h"


_Thread_local int hash_key = 0x30;
_Thread_local apr_pool_t *pool = NULL;
_Thread_local apr_hash_t *hash = NULL;
_Thread_local apr_pool_t *reverse_pool = NULL;
_Thread_local apr_hash_t *reverse_hash = NULL;
_Thread_local BYTE thread_id;
apr_hash_t *hash_swap()
{
    apr_pool_t *reverse_pool = NULL;
    apr_hash_t *reverse_hash = NULL;

    apr_pool_create(&reverse_pool, NULL);
    reverse_hash = apr_hash_make(reverse_pool);
    if (reverse_hash == NULL)
    {
        perror("New hash creation error");
        return NULL;
    }

    apr_hash_index_t *hi;
    for (hi = apr_hash_first(pool, hash); hi; hi = apr_hash_next(hi))
    {
        const char *key;
        int *val;

        // Get the current key and value
        apr_hash_this(hi, (const void **)&key, NULL, (void **)&val);

        char *pKey = apr_palloc(reverse_pool, sizeof(int));
        if (pKey == NULL)
        {
            perror("Key allocation error");
            return NULL;
        }
        *pKey = *val;

        char *pVal = apr_pstrdup(reverse_pool, key);
        if (pVal == NULL)
        {
            perror("Value allocation error");
            return NULL;
        }

        // In the new hash table, the value becomes the key and the key becomes the value
        apr_hash_set(reverse_hash, pKey, APR_HASH_KEY_STRING, pVal);
    }

    return reverse_hash;
}

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

int hash_init(int key)
{
    hash_key = key; // for purpose of seeing an ascii char in the hex editor for initial keys
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

BYTE hash_save_tlv(const char *filename, apr_pool_t *pool, apr_hash_t *hash)
{
    if (tlv_init_output_file(filename) != ERROR_NONE)
        return ERROR_TLV_FILE_OPEN;

    // Write hash map to file
    apr_hash_index_t *hi;
    for (hi = apr_hash_first(pool, hash); hi; hi = apr_hash_next(hi))
    {
        const char *key;
        int *val;

        // Get the current key and value
        apr_hash_this(hi, (const void **)&key, NULL, (void **)&val);

        // We use keys as strings and values as integers
        tlv_write_string(TLV_TOKEN_STRING, key);
        tlv_write_int(TLV_TOKEN_INT, *val);

        tlv_finilize();
        return ERROR_NONE;
    }
}

BOOL hash_load_tlv(const char *filename, apr_pool_t *pool, apr_hash_t *hash)
{

    // TODO
    return ERROR_NONE;
}
