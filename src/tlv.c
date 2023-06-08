#include "../include/tlv.h"
#include "jansson.h"
#include "../include/hash.h"

char tlv_file_name[MAX_FILE_LENGTH];
FILE *tlv_output_file;
BYTE *encode_tlv(uint8_t tag, uint16_t key, const void *data, uint16_t data_length, uint16_t *encodedLength)
{
    // Calculate the length required for the TLV encoding
    uint16_t length = sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t) + data_length;

    // Allocate memory for the encoded TLV data
    BYTE *encoded_data = (BYTE *)malloc(length);
    if (encoded_data == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for encoded TLV data\n");
        return NULL;
    }

    // Encode the TLV data
    BYTE *p = encoded_data;
    *p++ = (BYTE)tag;
    memcpy(p, &key, sizeof(key));
    p = p + sizeof(key);
    memcpy(p, &data_length, sizeof(data_length));
    p = p + sizeof(data_length);
    memcpy(p, data, data_length);

    // Set the encoded length
    *encodedLength = length;

    return encoded_data;
}

BOOL tlv_init_file(const char *filename)
{


    if (filename != NULL)
    {
        strcpy(tlv_file_name, filename); // TODO check size
        printf("Opening tlv file for writing [%s]\n", tlv_file_name);
        tlv_output_file = fopen(tlv_file_name, "wb");
        if (tlv_output_file == NULL)
            return ERROR_TLV_FILE_OPEN;
        else
            return ERROR_NONE;
    }
    else
    {
        return ERROR_NO_TLV_FILE;
    }
}
BOOL tlv_write_int(uint16_t key, int value)
{

    uint16_t encoded_length;
    BYTE *encoded_data = encode_tlv(TLV_TOKEN_INT, key, &value, sizeof(int), &encoded_length);
    if (encoded_data == NULL)
    {
        // fclose(tlv_output_file);
        return ERROR_TLV_MALLOC;
    }
    fwrite(encoded_data, sizeof(BYTE), encoded_length, tlv_output_file);
    free(encoded_data);
    return ERROR_NONE;
}
BOOL tlv_write_string(uint16_t key, const char *value)
{

    uint16_t encoded_length;
    BYTE *encoded_data = encode_tlv(TLV_TOKEN_STRING, key, value, strlen(value) + 1, &encoded_length);
    if (encoded_data == NULL)
    {
        // fclose(tlv_output_file);
        return ERROR_TLV_MALLOC;
    }
    fwrite(encoded_data, sizeof(BYTE), encoded_length, tlv_output_file);
    free(encoded_data);
    return ERROR_NONE;
}
BOOL tlv_write_bool(uint16_t key, BOOL value)
{

    uint16_t encoded_length;
    BYTE *encoded_data = encode_tlv(TLV_TOKEN_BOOL, key, &value, sizeof(BOOL), &encoded_length);
    if (encoded_data == NULL)
    {
        //  fclose(tlv_output_file);
        return ERROR_TLV_MALLOC;
    }
    fwrite(encoded_data, sizeof(BYTE), encoded_length, tlv_output_file);
    free(encoded_data);
    return ERROR_NONE;
}

BOOL tlv_write_start()
{

    uint16_t encoded_length;
    BYTE *nl = encode_tlv(TLV_TOKEN_LINE, 0, 0, 0, &encoded_length);
    if (nl == NULL)
    {
        // fclose(tlv_output_file);
        return ERROR_TLV_MALLOC;
    }
    fwrite(nl, sizeof(BYTE), encoded_length, tlv_output_file);
    free(nl);
    return ERROR_NONE;
}
long tlv_file_size(const char *filename)
{

    FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        perror("Error opening file");
        printf("Failed to open file [%s]\n", filename);
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);

    return size;
}
int tlv_read_file(const char *filename)
{
    FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        perror("Failed to open file");
        return ERROR_NO_TLV_FILE;
    }

    json_t *json_obj = json_object(); // Initialize the JSON object

    while (!feof(file))
    {
        BYTE tag;
        uint16_t key, length;

        if (fread(&tag, sizeof(BYTE), 1, file) != 1)
        {
            if (feof(file))
                break;
            perror("Failed to read tag");
            fclose(file);
            return ERROR_TLV_FORMAT;
        }

        if (fread(&key, sizeof(uint16_t), 1, file) != 1)
        {
            perror("Failed to read key");
            fclose(file);
            return ERROR_TLV_FORMAT;
        }

        if (fread(&length, sizeof(uint16_t), 1, file) != 1)
        {
            perror("Failed to read length");
            fclose(file);
            return ERROR_TLV_FORMAT;
        }

        // Allocate buffer for data
        BYTE *buffer = (BYTE *)malloc(length);
        if (buffer == NULL)
        {
            perror("Failed to allocate buffer");
            fclose(file);
            return ERROR_TLV_BUFFER_ALLOCATION_FAIL;
        }

        if (fread(buffer, sizeof(BYTE), length, file) != length)
        {
            perror("Failed to read data");
            free(buffer);
            fclose(file);
            return ERROR_TLV_FORMAT;
        }

        // Start handling the TLV data
        const char *key_name = hash_get_value((int)key);
        json_t *value;

        switch (tag)
        {
        case TLV_TOKEN_INT:
            value = json_integer(*(int *)buffer);
            break;
        case TLV_TOKEN_BOOL:
            value = json_boolean(*(BOOL *)buffer);
            break;
        case TLV_TOKEN_STRING:
            value = json_string((char *)buffer);
            break;
        default:
            fprintf(stderr, "Unknown tag value\n");
            free(buffer);
            fclose(file);
            return ERROR_TLV_FORMAT;
        }

        json_object_set_new(json_obj, key_name, value);

        free(buffer);
    }

    char *json_string = json_dumps(json_obj, JSON_INDENT(2)); // Convert to formatted string
    printf("%s\n", json_string);                              // Print the JSON string

    // Clean up
    free(json_string);
    json_decref(json_obj);

    fclose(file);
    return ERROR_NONE;
}
BOOL tlv_finilize()
{
    int result = ERROR_NONE;
    if (tlv_output_file != NULL)
    {

        result = fclose(tlv_output_file);
        tlv_output_file = NULL;
    }
    return result;
}
