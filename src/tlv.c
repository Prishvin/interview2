#include "../include/tlv.h"

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
#ifdef TEST_FLAG
    return ERROR_NONE;
#endif

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
        fclose(tlv_output_file);
        return ERROR_TLV_MALLOC;
    }
    fwrite(encoded_data, sizeof(BYTE), encoded_length, tlv_output_file);
    free(encoded_data);
    return ERROR_NONE;
}
BOOL tlv_write_string(uint16_t key, const char *value)
{
#ifdef TEST_FLAG
    return ERROR_NONE;
#endif
    uint16_t encoded_length;
    BYTE *encoded_data = encode_tlv(TLV_TOKEN_STRING, key, value, strlen(value) + 1, &encoded_length);
    if (encoded_data == NULL)
    {
        fclose(tlv_output_file);
        return ERROR_TLV_MALLOC;
    }
    fwrite(encoded_data, sizeof(BYTE), encoded_length, tlv_output_file);
    free(encoded_data);
    return ERROR_NONE;
}
BOOL tlv_write_bool(uint16_t key, BOOL value)
{
#ifdef TEST_FLAG
    return ERROR_NONE;
#endif
    uint16_t encoded_length;
    BYTE *encoded_data = encode_tlv(TLV_TOKEN_BOOL, key, &value, sizeof(BOOL), &encoded_length);
    if (encoded_data == NULL)
    {
        fclose(tlv_output_file);
        return ERROR_TLV_MALLOC;
    }
    fwrite(encoded_data, sizeof(BYTE), encoded_length, tlv_output_file);
    free(encoded_data);
    return ERROR_NONE;
}

BOOL tlv_write_start()
{
#ifdef TEST_FLAG
    return ERROR_NONE;
#endif
    uint16_t encoded_length;
    BYTE *nl = encode_tlv(TLV_TOKEN_LINE, 0, 0, 0, &encoded_length);
    if (nl == NULL)
    {
        fclose(tlv_output_file);
        return ERROR_TLV_MALLOC;
    }
    fwrite(nl, sizeof(BYTE), encoded_length, tlv_output_file);
    free(nl);
    return ERROR_NONE;
}
long tlv_file_size(const char *filename)
{
#ifdef TEST_FLAG
    return ERROR_NONE;
#endif
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
    /*
    // Open the binary file for reading
    long filesize = tlv_file_size(filename);
    printf("TLV file size is %d bytes", filesize);
    if (filesize < TLV_MIN_FILE_SIZE)
    {
        fprintf(stderr, "Failed to open the file\n");
        return ERROR_TLV_FILE_TOO_SMALL;
    }
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open the file\n");
        return ERROR_NO_TLV_FILE;
    }

    // Allocate a buffer to hold the read data

    BYTE *buffer = (BYTE *)malloc(READ_BUFFER_LENTH);
    if (buffer == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for buffer\n");
        fclose(file);
        return ERROR_TLV_BUFFER_ALLOCATION_FAIL;
    }

    // Read N bytes from the file
    long total_bytes = 0;
    json_t *json = NULL;
    BYTE token;
    uint16_t length;
    uint16_t key;
    BYTE nbytes;
    char *jsonString;
    while (total_bytes < filesize)
    {
        nbytes = fread(buffer, sizeof(BYTE), 1, file); // read tag
        if (nbytes > 0)
        {
            token = buffer[0];
            total_bytes = total_bytes + nbytes;
        }
        else
        {
            goto FINALLY;
        }

        nbytes = fread(buffer, 1, sizeof(uint16_t), file); // read key
        if (nbytes > 0)
        {
            memcpy(&key, buffer, sizeof(key));
            total_bytes = total_bytes + nbytes;
        }
        else
        {
            goto FINALLY;
        }

        nbytes = fread(buffer, 1, sizeof(uint16_t), file); // read data length
        if (nbytes > 0)
        {
            memcpy(&length, buffer, sizeof(length));
            total_bytes = total_bytes + nbytes;
        }
        else
        {
            goto FINALLY;
        }

        nbytes = fread(buffer, sizeof(BYTE), length, file); // check nbytes
        if (nbytes == length)
        {
            total_bytes = total_bytes + nbytes;
        }
        else
        {
            goto FINALLY;
        }

        switch (token)
        {
        case TLV_TOKEN_LINE:
            if (json != NULL)
            {
                jsonString = json_dumps(json, JSON_INDENT(2));
                if (jsonString == NULL)
                {
                    fprintf(stderr, "Failed to convert JSON object to string\n");
                    json_decref(json);
                    return 1;
                }
                printf("JSON Object:\n%s\n", jsonString);
                free(jsonString);
                json_decref(json);
                json = NULL;
            }
            json = json_object();
            if (json == NULL)
            {
                fprintf(stderr, "Failed to create JSON object\n");
                goto FINALLY;
            }
            break;
        case TLV_TOKEN_INT:
            int *n = (int *)buffer;
            json_object_set_new(json, "age", json_integer(&n));
            break;
        case TLV_TOKEN_BOOL:
            BOOL b = (BOOL)buffer;
            json_object_set_new(json, "isStudent", json_boolean(b));
            break;
        case TLV_TOKEN_STRING:

            json_object_set_new(json, "name", json_string(buffer));
            break;
        }
    }

    fclose(file);
    free(buffer);
    free(jsonString);
    json_decref(json);

    return ERROR_NONE;
FINALLY:
    json_decref(json);
    free(jsonString);
    return ERROR_TLV_FORMAT;*/
    return 0;
}
BOOL tlv_finilize()
{
    int result = ERROR_NONE;
    if (tlv_output_file != NULL)
        result = fclose(tlv_output_file);
    return result;
}
