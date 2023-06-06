#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include "hash.h"
#include "common.h"
#include "tlv.h"
#include <assert.h>
#include "hash.h"
#include <threads.h>

#define MAX_LINE_LENGTH 2048

void iterate_json_object(json_t *json, FilePart *part)
{
    char *key;
    json_t *value;

    // hash_print();
    json_object_foreach(json, key, value)
    {
        int key_value = hash_get_key(key);
        if (key_value < 0)
        {
            key_value = hash_key;
            hash_add(key, key_value);

            printf("[%d] New hash %d - %s\n", part->id, hash_key, key);
            hash_key++;
        }
        else
        {
            printf("[%d] Duplicate %s\n", part->id, key);
        }
        // Check the type of the value and handle accordingly
        if (json_is_string(value))
        {
            const char *str_value = json_string_value(value);
            tlv_write_string(key_value, str_value);
            printf("[%d] Value (string): %s\n", part->id, str_value);
        }
        else if (json_is_integer(value))
        {
            int int_value = json_integer_value(value);
            tlv_write_int(key_value, int_value);
            printf("[%d] Value (integer): %d\n", part->id, int_value);
        }
        else if (json_is_boolean(value))
        {
            BOOL bool_value = json_boolean_value(value);
            tlv_write_bool(key_value, bool_value);
            printf("[%d] Value (boolean): %s\n", part->id, bool_value ? "true" : "false");
        }
        else if (json_is_null(value))
        {
            printf("[%d] Value: null\n", part->id);
        }
        else
        {
            printf("[%d] Value: (unknown type)\n", part->id);
        }
    }
}

int read_json_part(void *arg)
{
    if (arg == NULL)
        return;

    int ret = hash_init(0x31);
    if (ret != ERROR_NONE)
    {
        printf("Hash table init failed.\n");
        return ret;
    }

    if (ret != ERROR_NONE)
    {
        printf("TLV file opening failed.\n");
        return ret;
    }
    FilePart *part = (FilePart *)arg;
    if (part == NULL)
    {
        printf("Invalid thread argument\n");
        return ERROR_INVALID_THREAD_ELEMENT;
    }
    char buf[MAX_FILE_LENGTH];
    sprintf(buf, "%s%d%s", tlv_file_name, part->id, TLV_EXT);

    ret = tlv_init_output_file(buf);
    if (ret != ERROR_NONE)
        if (ret != ERROR_NONE)
        {
            printf("[%d] Error opening tlv file\n", part->id);
            return ret;
        }

    ret = tlv_write_line_start(buf);
    if (ret != ERROR_NONE)
    {
        printf("[%d] Error writing tlv file\n", part->id);
        return ret;
    }

    FILE *file = fopen(part->input_file, "r");
    if (file == NULL)
    {
        printf("]%d] Thread, start [%d], opening file %s fail", part->id, part->start_pos, part->input_file);
    }

    json_error_t error;

    char line[MAX_LINE_LENGTH];
    fseek(file, part->start_pos, SEEK_SET);
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

        iterate_json_object(json, part);

        json_decref(json);
        printf("%s\n", line);
    }
    tlv_finilize();
    fclose(file);
}
void read_json_file(const char *file_name, int nthreads)
{
    thrd_t *threads = malloc(sizeof(thrd_t) * nthreads);
    FilePart *thread_args = malloc(sizeof(FilePart) * nthreads);
    int rc, i;

    // Create the threads
    for (i = 0; i < nthreads; i++)
    {
        FilePart part; // TODO: divide_file(file_name, nthreads, i);
        part.input_file = file_name;
        thread_args[i] = part; // Copy the data to the array

        printf("Creating thread %d\n", i);
        rc = thrd_create(&threads[i], read_json_part, (void *)&thread_args[i]);
        if (rc != thrd_success)
        {
            fprintf(stderr, "Error: unable to create thread, %d\n", rc);
            exit(-1);
        }
    }

    // Wait for each thread to complete
    for (i = 0; i < nthreads; i++)
    {
        thrd_join(threads[i], NULL);
    }

    free(threads);
    free(thread_args);
}

int main(int argc, char *argv[])
{
    char *input_file = NULL;
    char *output_tlv_file = "output.tlv";
    char *output_dictionary_file = "dictionary.tlv";
    size_t n_threads = 4;

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
        else if (strcmp(argv[i], "--nrthreads") == 0)
        {
            n_threads = atoi(argv[++i]);
        }
    }

    if (!input_file)
    {
        printf("Please provide an input file using --input.\n");
        printf("Usage: %s --input json_to_process --output output_tlv_file --dic output_dictionary_file\n", argv[0]);
        return 0;
    }

    read_json_file(input_file, n_threads);

    // write dictionary to tlv file
    int ret = hash_save_tlv(output_dictionary_file, pool, hash);
    if (ret != ERROR_NONE)
    {
        printf("TLV file opening failed.\n");
        return ret;
    }
    hash_destroy();

    return 0;
}
