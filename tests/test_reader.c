#include <CUnit/Basic.h>
#include "../include/reader.h"
#include <stdio.h>
#include <unistd.h>
#include <tlv.h>

void test_tlv_rw(void)
{
   // Create temporary TLV file
   char tlv_file[] = "/tmp/tlvfile-XXXXXX";
   int fd_tlv = mkstemp(tlv_file);
   if (fd_tlv == -1)
   {
      // Handle error
      return;
   }

   FILE *file;
   BOOL res = tlv_init_file(tlv_file, &file);
   CU_ASSERT_TRUE(res == ERROR_NONE);

   // write data to TLV file
   int int_val = 123;
   BOOL bool_val = true;
   const char *str_val = "Hello World!";
   tlv_write_int(1, int_val, file);
   tlv_write_bool(2, bool_val, file);
   tlv_write_string(3, str_val, file);

   // close the file
   tlv_finilize(file);

   // reopen the file for reading
   FILE *read_file = fopen(tlv_file, "rb");
   CU_ASSERT_PTR_NOT_NULL(read_file);

   // read and compare the data
   BYTE buffer[1024];
   fread(buffer, sizeof(BYTE), 1, read_file);        
   CU_ASSERT_TRUE(buffer[0] == TLV_TOKEN_INT);         
   fread(buffer, sizeof(uint16_t), 1, read_file);       
   CU_ASSERT_TRUE(buffer[0] == 1);                     
   fread(buffer, sizeof(uint16_t), 1, read_file);       
   fread(buffer, sizeof(BYTE), sizeof(int), read_file);
   CU_ASSERT_TRUE(*(int *)buffer == int_val);           // check value

   fread(buffer, sizeof(BYTE), 1, read_file);           
   CU_ASSERT_TRUE(buffer[0] == TLV_TOKEN_BOOL);         
   fread(buffer, sizeof(uint16_t), 1, read_file);     
   CU_ASSERT_TRUE(buffer[0] == 2);                      
   fread(buffer, sizeof(uint16_t), 1, read_file);       
   fread(buffer, sizeof(BYTE), sizeof(BOOL), read_file); 
   CU_ASSERT_TRUE(buffer[0] == bool_val);                 // check value

   fread(buffer, sizeof(BYTE), 1, read_file);                  
   CU_ASSERT_TRUE(buffer[0] == TLV_TOKEN_STRING);              
   fread(buffer, sizeof(uint16_t), 1, read_file);              
   CU_ASSERT_TRUE(buffer[0] == 3);                              
   fread(buffer, sizeof(uint16_t), 1, read_file);               
   fread(buffer, sizeof(BYTE), strlen(str_val) + 1, read_file); 
   CU_ASSERT_TRUE(strcmp((const char *)buffer, str_val) == 0);  // check value

   // close the read file
   fclose(read_file);

   // remove the file
   remove(tlv_file);
}



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
      close(fd_in);
      close(fd_out);
      return;
   }

   fprintf(fp_in, "{ \"key1\": \"data\" }\n");
   fprintf(fp_in, "{ \"key4\": \"data\" }\n");
   fprintf(fp_in, "{ \"key5\": 2 }\n");
   fprintf(fp_in, "{ \"key6\": true }\n");
   fprintf(fp_in, "{ \"key7\": false, \"key8\": \"abcde\"  }\n");

   fclose(fp_in);

   int ret = hash_init();
   if (ret != ERROR_NONE)
   {
      printf("Hash table init failed.\n");
      return;
   }

   size_t ntokens;
   read_json_file(input_file, output_file, dic_file, &ntokens);

   CU_ASSERT(ntokens == 6);
   hash_init();
   hash_load_tlv(dic_file, pool, hash);
   json_t *actual_json = NULL;
   tlv_read_json(output_file, &actual_json);

   hash_destroy();

   // Check specific keys
   CU_ASSERT_TRUE(hash_key_present("key1"));
   CU_ASSERT_TRUE(hash_key_present("key4"));
   CU_ASSERT_TRUE(hash_key_present("key5"));
   CU_ASSERT_TRUE(hash_key_present("key6"));
   CU_ASSERT_TRUE(hash_key_present("key7"));
   CU_ASSERT_TRUE(hash_key_present("key8"));
   CU_ASSERT_TRUE(!hash_key_present("keyX"));

   // Clean up
   remove(input_file);
   remove(output_file);
   remove(dic_file);
}

int main()
{
   CU_pSuite tlvSuite = NULL;
   CU_pSuite jsonSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add the tlv_test_suite to the registry */
   tlvSuite = CU_add_suite("tlv_test_suite", 0, 0);
   if (NULL == tlvSuite)
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the tlv_test_suite */
   if ((NULL == CU_add_test(tlvSuite, "test_tlv_rw", test_tlv_rw)) ||
       (NULL == CU_add_test(tlvSuite, "test_read_json_file", test_read_json_file)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the json_test_suite to the registry */
   jsonSuite = CU_add_suite("json_test_suite", 0, 0);
   if (NULL == jsonSuite)
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the test_read_json_file to the json_test_suite */
   if (NULL == CU_add_test(jsonSuite, "test_read_json_file", test_read_json_file))
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
