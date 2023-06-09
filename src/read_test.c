#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include "../include/tlv.h"
#include "../include/reader_test.h"
// Assuming these are the correct includes, adjust as necessary
#include "../include/reader.h"
#include "../include/hash.h"

int reader_test()
{
    // Create a temp file
    char temp_file[] = "/tmp/tempfile-XXXXXX";
    mkstemp(temp_file);

    // Open file for writing
    FILE *fp = fopen(temp_file, "w");
    if (fp == NULL)
    {
        perror("Unable to open file!");
        exit(1);
    }

    // Write separate JSON objects to the file
    char *json1 = "{ \"key1\": \"data\" }\n";
    char *json2 = "{ \"key4\": \"data\" }\n";
    char *json3 = "{ \"key5\": 2 }\n";
    char *json4 = "{ \"key6\": true }\n";
    char *json5 = "{ \"key7\": false, \"key8\": \"abcde\"  }\n";
    char *original_jsons[] = {json1, json2, json3, json4, json5};
    size_t json_count = sizeof(original_jsons) / sizeof(original_jsons[0]);
    fprintf(fp, "%s", json1);
    fprintf(fp, "%s", json2);
    fprintf(fp, "%s", json3);
    fprintf(fp, "%s", json4);
    fprintf(fp, "%s", json5);
    fclose(fp);

    printf("Written JSONs to file:\n");

    // Initialize the hash table
    int ret = hash_init();
    if (ret != ERROR_NONE)
    {
        printf("Hash table init failed.\n");
        return ret;
    }

    // Read JSON file and write to TLV file
    size_t ntokens;
    char output_file[] = "/tmp/outfile-XXXXXX";
    char dic_file[] = "/tmp/dicfile-XXXXXX";
    mkstemp(output_file);
    mkstemp(dic_file);
    read_json_file(temp_file, output_file, dic_file, &ntokens);
    hash_save_tlv(dic_file, pool, hash);
    hash_destroy();

    // Reinitialize the hash table
    ret = hash_init();
    if (ret != ERROR_NONE)
    {
        printf("Hash table re-init failed.\n");
        return ret;
    }

    // Load hash from the TLV file
    hash_load_tlv(dic_file, pool, hash);

    // Read TLV back to JSON
    json_t *read_back_json = NULL;
    
    tlv_read_json(output_file, &read_back_json);

    // Print JSON read back
    char *json_str = json_dumps(read_back_json, 0);
    printf("JSON Read back from tvl file: %s\n", json_str);

    for (int i = 0; i < json_count; i++)
    {
        json_t *original_json = json_loads(original_jsons[i], 0, NULL);
        json_t *read_back= json_array_get(read_back_json, i);
        char* original_str = json_dumps(original_json, JSON_ENCODE_ANY);
        char* read_str = json_dumps(read_back, JSON_ENCODE_ANY);
        if (json_equal(original_json, read_back))
            printf("Original and read JSON data are the same.%s %s\n", original_str, read_str);
        else
            printf("Original and read JSON data are NOT the same. %s %s\n", original_str, read_str);

        // Decrease reference count of original json
        if (original_json != NULL)
            json_decref(original_json);
    }


    // Cleanup
    free(json_str);
    remove(temp_file);
    remove(output_file);
    remove(dic_file);
    hash_destroy();
    if (read_back_json != NULL)
        json_decref(read_back_json);

    return 0;
}
