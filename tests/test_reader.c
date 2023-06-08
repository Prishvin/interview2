#include <CUnit/Basic.h>
#include "../include/reader.h"
#include <stdio.h>
#include <unistd.h>

void test_read_json_file(void)
{
   // Create temporary JSON files
   char input_file[] = "/tmp/infile-XXXXXX";
   char output_file[] = "/tmp/outfile-XXXXXX";
   char dic_file[] = "/tmp/dicfile-XXXXXX";

   int fd_in = mkstemp(input_file);
   int fd_out = mkstemp(output_file);
   int fd_dic = mkstemp(dic_file);

   if (fd_in == -1 || fd_out == -1 || fd_dic == -1)
   {
      // Handle error
      return;
   }

   FILE *fp_in = fdopen(fd_in, "w");

   if (fp_in == NULL)
   {
      // Handle error
      close(fd_in);
      close(fd_out);
      return;
   }

   // Let's write some data to input and dictionary files
   fprintf(fp_in, "{ \"test\": \"data\" }\n");


   fclose(fp_in);


   // Call function under test
   size_t ntokens;
   read_json_file(input_file, output_file, dic_file, &ntokens);

   // Add assertions
   CU_ASSERT(ntokens == 1); // Assuming 1 token is correctly identified
   // Add more assertions as needed

   // Clean up
   remove(input_file);
   remove(output_file);
   remove(dic_file);
}

// Rest of your main() function follows...
