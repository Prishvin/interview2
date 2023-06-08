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
    printf("TVL json data:\n");
    tlv_read_json(output_file, &read_back_json);

    // Print JSON read back
    char *json_str = json_dumps(read_back_json, 0);
    printf("JSON Read from file: %s\n", json_str);
    
    printf("Original data:\n");
    printf("%s", json1);
    printf("%s", json2);
    printf("%s", json3);
    printf("%s", json4);
    printf("%s", json5);
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
