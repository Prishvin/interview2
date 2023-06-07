#include <CUnit/Basic.h>
#include "../include/reader.h"
#include <stdio.h>

void test_read_json_file(void) {
    // First, let's create an in-memory JSON file
    char* filename = tmpnam(NULL);
    FILE* fp = fopen(filename, "w");
    fprintf(fp, "{ \"test\": \"data\" }\n");
    fclose(fp);

    // Call function under test
    int result = read_json_file(filename);

    // Add assertions
    CU_ASSERT(result == 0);  // Assuming 0 represents success in your function
    // Add more assertions as needed
    remove(filename);
}

int main() {
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("json_test_suite", 0, 0);
   if (NULL == pSuite) {
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
