#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include "hash.h"
#include "common.h"
#include "tlv.h"
#include <assert.h>

#define MAX_LINE_LENGTH 1024

void iterate_json_object(json_t *json)
{
    char *key;
    json_t *value;
    tlv_write_start();
    hash_print();
    json_object_foreach(json, key, value)
    {
        int key_value = hash_get_key(key);
        if (key_value < 0)
        {
            key_value = hash_key;
            hash_add(key, key_value);
           
            printf("New hash %d - %s\n", hash_key, key);
             hash_key++;
        }
        else
        {
            printf("Duplicate %s\n", key);
        }
        // Check the type of the value and handle accordingly
        if (json_is_string(value))
        {

            const char *str_value = json_string_value(value);
            tlv_write_string(key_value, str_value);
            printf("Value (string): %s\n", str_value);
        }
        else if (json_is_integer(value))
        {
            int int_value = json_integer_value(value);
            tlv_write_int(key_value, int_value);
            printf("Value (integer): %d\n", int_value);
        }
        else if (json_is_boolean(value))
        {
            BOOL bool_value = json_boolean_value(value);
            tlv_write_bool(key_value, bool_value);
            printf("Value (boolean): %s\n", bool_value ? "true" : "false");
        }
        else if (json_is_null(value))
        {
            printf("Value: null\n");
        }
        else
        {
            printf("Value: (unknown type)\n");
        }
    }
}
void read_json_file(const char *file_name)
{
    FILE *file = fopen(file_name, "r");
    if (file == NULL)
    {
        printf("File '%s' not found.\n", file_name);
        return;
    }
    json_error_t error;

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file))
    {
        // Remove the trailing newline character
        line[strcspn(line, "\n")] = '\0';
        json_t *json = json_loads(line, 0, &error);
        if (!json)
        {
            fprintf(stderr, "Error parsing JSON: %s\n", error.text);
            return 1;
        }

        if (!json_is_object(json))
        {
            fprintf(stderr, "JSON is not an object\n");
            json_decref(json);
            return 1;
        }

        iterate_json_object(json);

        json_decref(json);
        printf("%s\n", line);
    }

    fclose(file);
}


int main(int argc, char *argv[])
{
    //test_hash();
    if (argc < 2)
    {
        printf("Please provide a file name as an argument.\n");
        return 0;
    }

    int ret = hash_init();
    if (ret != ERROR_NONE)
    {
        printf("Hash table init failed.\n");
        return ret;
    }

    ret = tlv_init_file("example.tlv");
    if (ret != ERROR_NONE)
    {
        printf("TLV file opening failed.\n");
        return ret;
    }

    const char *file_name = argv[1];
    read_json_file(file_name);
    tlv_finilize();

    //ret = tlv_read_file("example.tlv");
    hash_save_tlv("dictionary.tlv", pool, hash);
    if (ret != ERROR_NONE)
    {
        printf("TLV file opening failed.\n");
        return ret;
    }

    return 0;
}
