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
   fprintf(fp_in, "{ \"key1\": \"data\" }\n");
   fprintf(fp_in, "{ \"key4\": \"data\" }\n");
   fprintf(fp_in, "{ \"key5\": 2 }\n");

   fclose(fp_in);

   int ret = hash_init();
   if (ret != ERROR_NONE)
   {
      printf("Hash table init failed.\n");
      return;
   }

   // Call function under test
   size_t ntokens;
   read_json_file(input_file, output_file, dic_file, &ntokens);

   // Add assertions
   CU_ASSERT(ntokens == 3); // Assuming 1 token is correctly identified
   // Add more assertions as needed

   // Clean up
   remove(input_file);
   remove(output_file);
   remove(dic_file);
}

// Rest of your main() function follows...

int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("json_test_suite", 0, 0);
   if (NULL == pSuite)
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "test_read_json_file", test_read_json_file)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   // Run all tests using the basic interface
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();

   return CU_get_error();
}
