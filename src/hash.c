#include "../include/hash.h"

int hash_key = 0x30;
apr_pool_t *pool = NULL;
apr_hash_t *hash = NULL;
apr_pool_t *reverse_pool = NULL;
apr_hash_t *reverse_hash = NULL;

size_t hash_count()
{
    int n = 0;
    if (hash != NULL)
        n = apr_hash_count(hash);
    return n;
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

int hash_init()
{
    hash_key = 0x30; // for purpose of seeing an ascii char in the hex editor for initial keys
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

int hash_get_key(const char *key)
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

    if (tlv_init_file(filename) != ERROR_NONE)
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
    }

    tlv_finilize(filename);
    return ERROR_NONE;
}

BOOL hash_load_tlv(const char *filename, apr_pool_t *pool, apr_hash_t *hash)
{
    FILE *file = fopen(filename, "rb");
    apr_hash_clear(hash);
    if (file == NULL) {
        perror("Failed to open file");
        return ERROR_TLV_FILE_OPEN;
    }

    char* key_name = NULL;
    int value;

    while (!feof(file)) {
        BYTE tag;
        uint16_t key;
        uint16_t length;

        // Read tag
        if (fread(&tag, sizeof(BYTE), 1, file) != 1) {
            if (feof(file)) break;
            perror("Failed to read tag");
            fclose(file);
            return ERROR_TLV_FORMAT;
        }

        // Read key
        if (fread(&key, sizeof(uint16_t), 1, file) != 1) {
            perror("Failed to read key");
            fclose(file);
            return ERROR_TLV_FORMAT;
        }

        // Read length
        if (fread(&length, sizeof(uint16_t), 1, file) != 1) {
            perror("Failed to read length");
            fclose(file);
            return ERROR_TLV_FORMAT;
        }

        // Allocate buffer for data
        BYTE *buffer = (BYTE *)malloc(length);
        if (buffer == NULL) {
            perror("Failed to allocate buffer");
            fclose(file);
            return ERROR_TLV_BUFFER_ALLOCATION_FAIL;
        }

        // Read data
        if (fread(buffer, sizeof(BYTE), length, file) != length) {
            perror("Failed to read data");
            free(buffer);
            fclose(file);
            return ERROR_TLV_FORMAT;
        }

        // Handle the TLV data
        switch (tag) {
            case TLV_TOKEN_STRING:
                key_name = apr_pstrdup(pool, (char *)buffer);
                break;
            case TLV_TOKEN_INT:
                value = *(int *)buffer;
                if (key_name != NULL) {
                    hash_add(key_name, value);  // Add to hash
                    key_name = NULL;  // Reset key_name after adding to hash
                }
                break;
            default:
                fprintf(stderr, "Unknown tag value\n");
                free(buffer);
                fclose(file);
                return ERROR_TLV_FORMAT;
        }

        free(buffer);
    }

    fclose(file);
    return ERROR_NONE;
}




// Function to swap keys and values in the hash table
apr_hash_t *hash_swap()
{
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
        apr_hash_set(reverse_hash, pKey, sizeof(int), pVal);  // use sizeof(int) instead of APR_HASH_KEY_STRING
    }

    return reverse_hash;
}

// Function to initialize reverse hash if it's not initialized yet
void hash_swap_init() {
    // call hash_swap function only if reverse_hash is not initialized yet
    if (reverse_hash == NULL) {
        reverse_hash = hash_swap();
    }
}

// Function to get string key by integer value
const char* hash_get_value(int key) {
    // make sure the hash_swap_init function is called first to initialize reverse_hash
    hash_swap_init();

    // use the integer key to get the string value
    const char *value = apr_hash_get(reverse_hash, &key, sizeof(int));

    // return the found string value, or NULL if no such key
    return value;
}
