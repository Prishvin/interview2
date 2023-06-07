#include "../include/common.h"
long get_file_size(const char *filename)
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

void calculate_file_parts(const char *filename, int n, FilePart *parts) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open the file\n");
        return;
    }

    // Calculate file size
    fseek(file, 0L, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Calculate size per part
    long size_per_part = file_size / n;

    // Calculate start and end position of each part
    for (int i = 0; i < n; i++) {
        parts[i].start_pos = (i == 0) ? 0 : parts[i - 1].end_pos + 1;
        parts[i].end_pos = (i == n - 1) ? file_size : (i + 1) * size_per_part;

        // Adjust end position to the nearest newline
        if (i != n - 1) {
            fseek(file, parts[i].end_pos, SEEK_SET);
            while (!feof(file) && fgetc(file) != '\n') {
                parts[i].end_pos++;
            }
        }
    }

    fclose(file);
}

