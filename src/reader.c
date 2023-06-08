#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include "../include/hash.h"
#include "../include/common.h"
#include "../include/tlv.h"
#include <assert.h>
#include "../include/reader.h"
#include <unistd.h>
#include "../include/reader_test.h"

#define MAX_LINE_LENGTH 2048

void iterate_json_object(json_t *json,  FILE* tlv_json_file )
{
    const char *key;
    const json_t *value;

    tlv_write_start_line(tlv_json_file);
    // hash_print();
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
            tlv_write_string(key_value, str_value, tlv_json_file);
            printf("Value (string): %s\n", str_value);
        }
        else if (json_is_integer(value))
        {
            int int_value = json_integer_value(value);
            tlv_write_int(key_value, int_value, tlv_json_file);
            printf("Value (integer): %d\n", int_value);
        }
        else if (json_is_boolean(value))
        {
            BOOL bool_value = json_boolean_value(value);
            tlv_write_bool(key_value, bool_value, tlv_json_file);
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

void read_json_part(void *arg)
{
    if (arg == NULL)
        return;
    FilePart *part = (FilePart *)arg;
    FILE *file = fopen(part->file_name, "r");
    if (file == NULL)
    {
        printf("Thread %ld, opening file %s fail", part->id, part->file_name);
    }
}
int read_json_file(const char *input_file, const char *output_file_name, const char *dic_file, size_t *ntokens)
{
    FILE *file = fopen(input_file, "r");
    FILE* output_file = NULL;
    if (file == NULL)
    {
        printf("File '%s' not found.\n", input_file);
        return ERROR_JSON_FILE_NOT_FOUND;
    }
    int ret = tlv_init_file(output_file_name, &output_file);
    if (ret != ERROR_NONE)
    {
        printf("TLV file opening failed.\n");
        return ret;
    }

    json_error_t error;

    // long file_size = get_file_size(file_name);

    char line[MAX_LINE_LENGTH];
    // if we would to read a file, which contains long lines, we would allocated line buffer manually
    // for now max length is MAX_LINE_LENGTH
    while (fgets(line, sizeof(line), file))
    {
        // Remove the trailing newline character
        line[strcspn(line, "\n")] = '\0';
        json_t *json = json_loads(line, 0, &error);
        if (!json)
        {
            fprintf(stderr, "Error parsing JSON: %s\n", error.text);
            return ERROR_JSON_PARSING_ERROR;
        }

        if (!json_is_object(json))
        {
            fprintf(stderr, "JSON is not an object\n");
            json_decref(json);
            return ERROR_JSON_PARSING_ERROR;
        }

        iterate_json_object(json, output_file);

        json_decref(json);
        printf("%s\n", line);
    }
    printf("hash records %zu\n", *ntokens);
    tlv_finilize(output_file);

    *ntokens = hash_count();
    ret = hash_save_tlv(dic_file, pool, hash);
    if (ret != ERROR_NONE)
    {
        printf("TLV file opening failed.\n");
        return ret;
    }

    fclose(file);
    return ERROR_NONE;
}
#ifndef TEST_FLAG

int main(int argc, char *argv[])
{
    char *input_file = NULL;
    char *output_tlv_file = "output.tlv";
    char *output_dictionary_file = "dictionary.tlv";
    // size_t n_threads = 4;
    BOOL run_read_test = FALSE;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--input") == 0)
        {
            input_file = argv[++i];
        }
        else if (strcmp(argv[i], "--output") == 0)
        {
            output_tlv_file = argv[++i];
        }
        else if (strcmp(argv[i], "--dic") == 0)
        {
            output_dictionary_file = argv[++i];
        }
          else if (strcmp(argv[i], "--test") == 0)
        {
            run_read_test = 1;
        }
    }

    if (run_read_test)
    {
        reader_test();  // Call your test function here
        return 0;
    }

    if (!input_file)
    {
        printf("Please provide an input file using --input.\n");
        printf("Usage: %s --input json_to_process --output output_tlv_file --dic output_dictionary_file\n", argv[0]);
        return 0;
    }
    int ret = hash_init();
    if (ret != ERROR_NONE)
    {
        printf("Hash table init failed.\n");
        return ret;
    }

    size_t nkeys;
    read_json_file(input_file, output_tlv_file, output_dictionary_file, &nkeys);

    // write dictionary to tlv file

    hash_destroy();

    return 0;
}

#endif