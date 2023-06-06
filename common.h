#ifndef COMMON_H
#define COMMON_H

#include <stdio.h> 
#include "stack.h"
#include "types.h"

#define TLV_EXT "tlv"

#define MAX_FILE_LENGTH 1024
#define READ_BUFFER_LENTH 1024

typedef struct {
    long start_pos;
    long end_pos;
    long line_count;
    size_t id;
    char* input_file;
    Stack *stack;

} FilePart;

long get_file_size(const char *filename);
void calculate_file_parts(const char *filename, int n, FilePart *parts) ;



#endif