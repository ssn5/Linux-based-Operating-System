#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "handlers.h"
#include "terminal.h"
#include "filesystem.h"
#include "system_calls.h"
#include "rtc.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
// int idt_test(){
// 	TEST_HEADER;

// 	int i;
// 	int result = PASS;
// 	for (i = 0; i < 10; ++i){
// 		if ((idt[i].offset_15_00 == NULL) && 
// 			(idt[i].offset_31_16 == NULL)){
// 			assertion_failure();
// 			result = FAIL;
// 		}
// 	}
// 	return result;
// }

/* Division Zero Test
 * 
 * Cause a zero division exception
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Zero Divsion Exception Invoked
 * Coverage: Exception Handler
 * Files: handlers.h/c
 */
int division_zero_test(){
	TEST_HEADER;

	unsigned int i, j, result;
	i = 5;
	j = 0;
	result = i/j; //divide number by 0 (should raise divide by zero exception 0)

	return result;
}

// /* System Call Test
//  * 
//  * Calls a system call 
//  * Inputs: None
//  * Outputs: PASS/FAIL
//  * Side Effects: System Call Invoked
//  * Coverage: System Call Linkage & Handler
//  * Files: handlers.h/c
//  */
// int system_call_test(){
// 	TEST_HEADER;
//     int val = 3;
//     asm volatile("movl %0, %%eax ;\ movl $1, %%ebx ;\ movl $1, %%ecx ;\ movl $1, %%edx ;\ int $0x80"
//         :
//         : "r" (val));
// 	// asm volatile("movl val, %eax ;\ int $0x80");	//$0x80: invoke system call
// 	printf("manic\n");
// 	return PASS;
// }


//use char for paging tests since, pointer to be 8 bits (a byte)

/* Paging Test 1
 * 
 * Checks start of video memory
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: Paging Initialization 
 * Files: paging.h/c, paging_helper.S
 */
int paging_test1(){
	TEST_HEADER;
	char val;
	char* addr = (char *) 0xB8000;	//0xB8000: start of video memory
	val = *addr;	//dereference memory address
	return PASS;
}

/* Paging Test 2
 * 
 * Checks video memory outside bounds (after end)
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: Throws page fault exception
 * Coverage: Paging Initialization 
 * Files: paging.h/c, paging_helper.S
 */
int paging_test2(){
	TEST_HEADER;
	char val;
	char* addr = (char *) 0xB9000;	//0xB9000: memory address right after end of vid mem
	val = *addr;	//dereference memory address
	return FAIL;
}

/* Paging Test 3
 * 
 * Checks end of video memory
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: Paging Initialization 
 * Files: paging.h/c, paging_helper.S
 */
int paging_test3(){
	TEST_HEADER;
	char val;
	char* addr = (char *) 0xB8FFF;	//0xB8FFF: end of video memory address
	val = *addr;	//dereference memory address
	return PASS;
}

/* Paging Test 4
 * 
 * Checks video memory outside bounds (before start)
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: Throws page fault exception
 * Coverage: Paging Initialization 
 * Files: paging.h/c, paging_helper.S
 */
int paging_test4(){
	TEST_HEADER;
	char val;
	char* addr = (char *) 0xB7FFF; //0xB7FFF: memory address right before start of vid mem
	val = *addr;	//dereference memory address
	return FAIL;
}

/* Paging Test 5
 * 
 * Checks start of kernel memory
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: Paging Initialization 
 * Files: paging.h/c, paging_helper.S
 */
int paging_test5(){
	TEST_HEADER;
	char val;
	char* addr = (char *) 0x400000; //0x400000: start of kernel memory
	val = *addr;	//dereference memory address
	return PASS;
}

/* Paging Test 6
 * 
 * Checks kernel memory outside bounds (before start)
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: Throws page fault exception
 * Coverage: Paging Initialization 
 * Files: paging.h/c, paging_helper.S
 */
int paging_test6(){
	TEST_HEADER;
	char val;
	char* addr = (char *) 0x3FFFFF;	//0x3FFFFF: memory address right before start of kernel mem
	val = *addr;	//dereference memory address
	return FAIL;
}

/* Paging Test 7
 * 
 * Checks end of kernel memory
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: Paging Initialization 
 * Files: paging.h/c, paging_helper.S
 */
int paging_test7(){
	TEST_HEADER;
	char val;
	char* addr = (char *) 0x7FFFFF;	//0x7FFFFF: end of kernel memory address
	val = *addr;	//dereference memory address
	return PASS;
}

/* Paging Test 8
 * 
 * Checks kernel memory outside bounds (after end)
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: Throws page fault exception
 * Coverage: Paging Initialization 
 * Files: paging.h/c, paging_helper.S
 */
int paging_test8(){
	TEST_HEADER;
	char val;
	char* addr = (char *) 0x800000;	//0x800000: memory address right after end of kernel mem
	val = *addr;	//dereference memory address
	return FAIL;
}

/* Checkpoint 2 tests */

// File System Tests

// /* test_read_data
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass/fail
// * SIDE EFFECTS: Testing read_data
// */
// int test_read_data_basic(){
// 	TEST_HEADER;
// 	unsigned i = 0;
// 	uint8_t buf[3000];
// 	uint8_t inode = 3;
// 	uint32_t offset = 0;
// 	uint32_t length = 300;
// 	read_data(inode, offset, buf, length);	
// 	for(i = 0; i < 3000; i++){
// 		putc(buf[i]);				// This just displays random junk characters on the screen
// 	}
// 	return PASS;
// }


// /* test_read_dentry_by_name
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass/fail
// * SIDE EFFECTS: Testing read dentry by name and index
// */
// int test_read_dentry_by_name(){
// 	dentry_t temp_dentry;
// 	unsigned i = 0;
// 	uint8_t* buf = (uint8_t*)"frame0.txt";
// 	read_dentry_by_name(buf, &temp_dentry);
// 	for(i = 0; i < 32; i++){
// 		if(temp_dentry.filename[i] != '\0'){
// 			putc(temp_dentry.filename[i]);
// 		}
// 		// putc(temp_dentry.filename[i]);
// 	}
// 	printf("\n");
// 	return PASS;
// }


// // File Test Functions

// /* test_file_open
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass/fail
// * SIDE EFFECTS: testing file_open
// */
// int test_file_open(){
// 	printf("Testing File Open");
// 	printf("\n");
// 	uint8_t* filename = (uint8_t*)"frame0.txt";
// 	int val = file_open(filename, 4);
// 	printf("Return value is: %d \n", val);
// 	return PASS;
// }

// /* test_file_close
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass/fail
// * SIDE EFFECTS: testing file_close*/
// int test_file_close_valid_fd(){
// 	printf("Testing File Close with Valid File Descriptor");
// 	printf("\n");
// 	int val = file_close(2);
// 	printf("For a valid fd, return val is: %d \n", val);
// 	return PASS;
// }


// /* test_file_read
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass/fail
// * SIDE EFFECTS: testing reading text file by name
// */
// int test_file_read_textfile(){
// 	// clear();
// 	unsigned i = 0;
// 	int size = 10000;
// 	uint8_t buf[size];
// 	for(i = 0; i < size; i++){
// 		buf[i] = NULL;
// 	}
// 	clear();
// 	uint8_t* filename = (uint8_t*)"frame0.txt";
// 	if (file_read(filename, buf, size) == -1) {
// 		return FAIL;
// 	}
// 	// for(i = 0; i < 300; i++){
// 	// 	putc(buf[i]);
// 	// }
// 	// putc("\n");
// 	// puts(buf);
// 	for(i = 0; i < size; i++){
// 		if(buf[i] != NULL){
// 			putc(buf[i]);
// 		}
// 	}
// 	return PASS;
// }


// /* test_file_read_executablefile
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass/fail
// * SIDE EFFECTS: testing reading executable file by name
// */
// int test_file_read_executablefile(){
// 	// clear();
// 	unsigned i = 0;
// 	int size = 10000;
// 	uint8_t buf[size];
// 	for(i = 0; i < size; i++){
// 		buf[i] = NULL;
// 	}
// 	clear();
// 	uint8_t* filename = (uint8_t*)"grep";
// 	if (file_read(filename, buf, size) == -1) {
// 		return FAIL;
// 	}
// 	// for(i = 0; i < 300; i++){
// 	// 	putc(buf[i]);
// 	// }
// 	// putc("\n");
// 	// puts(buf);
// 	for(i = 0; i < size; i++){
// 		if(buf[i] != NULL){
// 			putc(buf[i]);
// 		}
// 	}
// 	printf("\n");
// 	return PASS;
// }

// /* test_file_read_largefile
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass/fail
// * SIDE EFFECTS: testing reading executable file by name
// */
// int test_file_read_largefile(){
// 	// clear();
// 	unsigned i = 0;
// 	int size = 10000;
// 	uint8_t buf[size];
// 	for(i = 0; i < size; i++){
// 		buf[i] = NULL;
// 	}
// 	clear();
// 	uint8_t* filename = (uint8_t*)"verylargetextwithverylongname.txt";
// 	if (file_read(filename, buf, size) == -1) {
// 		return FAIL;
// 	}
// 	// for(i = 0; i < 300; i++){
// 	// 	putc(buf[i]);
// 	// }
// 	// putc("\n");
// 	// puts(buf);
// 	for(i = 0; i < size; i++){
// 		if(buf[i] != NULL){
// 			putc(buf[i]);
// 		}
// 	}
// 	return PASS;
// }

// /* test_file_write
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass/fail
// * SIDE EFFECTS: testing file close
// */
// int test_file_write(){
// 	printf("Testing File Write");
// 	printf("\n");
// 	int val = file_write(6, (const void*)"cat", 5);
// 	printf("Since this is a read-only file system, return value is: %d \n", val);
// 	return PASS;
// }

// // Directory Test Functions

// /* test_directory_open
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass/fail
// * SIDE EFFECTS: testing directory open
// */
// int test_directory_open(){
// 	printf("Testing Directory Open");
// 	printf("\n");
// 	uint8_t* filename = (uint8_t*)".";
// 	int val = directory_open(filename, 4);
// 	printf("Return value is: %d \n", val);
// 	return PASS;
// }

// /* test_directory_close_valid_fd
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass/fail
// * SIDE EFFECTS: testing directory close
// */
// int test_directory_close_valid_fd(){
// 	printf("Testing Directory Close with Valid File Descriptor");
// 	printf("\n");
// 	int val = directory_close(2);
// 	printf("For a valid fd, return val is: %d \n", val);
// 	return PASS;
// }

// /* test_directory_close_invalid_fd
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass/fail
// * SIDE EFFECTS: testing directory close
// */
// int test_directory_close_invalid_fd(){
// 	printf("Testing Directory Close with Invalid File Descriptor");
// 	printf("\n");
// 	int val = directory_close(9);
// 	printf("For an invalid fd, return val is: %d \n", val);
// 	return PASS;
// }

// /* test_directory_read
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass/fail
// * SIDE EFFECTS: testing directory read
// */
// int test_directory_read(){
//     clear();
// 	printf("Testing Directory Read");
// 	printf("\n");
// 	unsigned i = 0;
// 	uint8_t buf[1000];
// 	uint8_t* filename = (uint8_t*)".";
// 	int8_t size = directory_read(filename, buf, 1000); 
// 	size++;			// done to remove warning
// 	size--;
// 	int count = 0;
// 	for(i = 0; i < (32*17); i++){
// 		putc(buf[i]);
// 		count++;
// 		if(count % 32 == 0){
// 			printf("\n");
// 		}
// 	}
// 	// for(i = 0; i < bootblock_ptr->directory_num; i++){
// 	// 	char buf1[32];
// 	// 	if(directory_read((uint8_t*)"cat", (uint8_t)buf1, 0) != -1){
// 	// 		printf("%s\n", buf1);
// 	// 	}
// 	// }
// 	// puts(buf);
// 	return PASS;
// }

// /* test_directory_write
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass/fail
// * SIDE EFFECTS: testing directory close
// */
// int test_directory_write(){
// 	printf("Testing Directory Write");
// 	printf("\n");
// 	int val = directory_write(6, (const void*)"cat", 5);
// 	printf("Since this is a read-only file system, return value is: %d \n", val);
// 	return PASS;
// }

// /* keyboard_test
// * INPUTS: none
// * OUTPUTS: print user buffer characters to screen
// * RETURN: none
// * SIDE EFFECTS: none
// */
// int keyboard_test(){
// 	unsigned char temp[128];
// 	unsigned int num, num_bytes;
// 	num = 128;
// 	//terminal_read(temp, num);
// 	while(1){
// 		num_bytes = terminal_read(temp, num);
// 		terminal_write(temp, num_bytes);
// 	}
// }

// /* rtc_write_test
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass/fail
// * SIDE EFFECTS: changes RTC frequency to 4 Hz.
// */
// int rtc_write_test(){
//     TEST_HEADER;

//     int i;
//     int fd;
//     printf("tests.c: RTC open call\n");
//     asm volatile("movl %1, %%eax ;\ movl %2, %%ebx ;\ movl $0, %%ecx ;\ movl $0, %%edx ;\ int $0x80 ;\  movl %%eax, %0"
//         : "=a"(fd)
//         : "a" (5), "b" ("rtc"));

//     for (i = 0; i < 10; i++) rtc_read(0,0,0);
//     printf("done for 2 hz\n");

//     // i = rtc_write((uint8_t*)4, 2); // set rtc frequency to 4 Hz
//     printf("tests.c: RTC write call\n");
//     asm volatile("movl %1, %%eax ;\ movl %2, %%ebx ;\ movl %3, %%ecx ;\ movl %4, %%edx ;\ int $0x80 ;\ movl %%eax, %0"
//         : "=r" (i)
//         : "a" (4), "b" (fd), "c" (8), "d" (4));
//     if (i == -1) return FAIL;
//     for (i = 0; i < 10; i++) rtc_read(0,0,0);
//     printf("done for 8 hz\n");

//     printf("tests.c: RTC write call\n");
//     asm volatile("movl %1, %%eax ;\ movl %2, %%ebx ;\ movl %3, %%ecx ;\ movl %4, %%edx ;\ int $0x80 ;\ movl %%eax, %0"
//         : "=r" (i)
//         : "a" (4), "b" (fd), "c" (1024), "d" (4));
//     if (i == -1) return FAIL;
//     for (i = 0; i < 10; i++) rtc_read(0,0,0);
//     printf("done for 1024 hz\n");

//     if (i == -1) return FAIL; // if fails to write, fail test
    
//     return PASS;

// }

// /* rtc_close_test
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass/fail
// * SIDE EFFECTS: changes RTC frequency to 4 Hz.
// */
// int rtc_close_test(){
//     TEST_HEADER;

//     int i;
//     int fd;
//     printf("tests.c: RTC open call\n");
//     asm volatile("movl %1, %%eax ;\ movl %2, %%ebx ;\ movl $0, %%ecx ;\ movl $0, %%edx ;\ int $0x80 ;\  movl %%eax, %0"
//         : "=a"(fd)
//         : "a" (5), "b" ("rtc"));

//     if (i == -1) return FAIL;

//     printf("tests.c: RTC close call\n");
//     asm volatile("movl %1, %%eax ;\ movl %2, %%ebx ;\ movl %3, %%ecx ;\ movl %4, %%edx ;\ int $0x80 ;\ movl %%eax, %0"
//         : "=r" (i)
//         : "a" (6), "b" (fd), "c" (0), "d" (0));
//     if (i == -1) return FAIL;
    

    
//     return PASS;

// }

// /* rtc_freq_test
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass
// * SIDE EFFECTS: Sets rtc frequency to all possible values.
// */
// int rtc_freq_test(){
//     TEST_HEADER;

//     printf("Running rtc_open now\n");
//     rtc_open(); // if not initialized, rtc_open (sanity check)
//     printf("rtc_open complete\n");
//     uint32_t i; // loop counter for diff frequencies
//     int j; // loop counter for printing char
//     int j_cond; // variable end condition for printing char
//     printf("Starting frequency tests. RTC will be set to each frequency for 2 seconds.\n");
//     for(i=2; i<=1024; ){ //max frequency allowed is 1024 Hz
//         clear();
//         rtc_write((uint8_t*)i, 4); // change frequency to current loop counter
//         printf("RTC frequency = %u Hz\n", i);
//         j_cond = 2*i; // sets end condition such that rtc remains at i Hz for 2 seconds
//         for (j = 0; j < j_cond; j++){
//             rtc_read(); // wait for rtc interrupt
//             printf("1"); // print char to show rtc interrupt complete
//         }
//         printf("\n");
//         i = i * 2; // get next possible frequency (always power of 2)
//     }
//     return PASS;
// }


// /* rtc_freq_test_generic_sys_call
// * INPUTS: none
// * OUTPUTS: none
// * RETURN: pass
// * SIDE EFFECTS: Sets rtc frequency to all possible values.
// */
// int rtc_freq_test_generic_sys_call(){
//     TEST_HEADER;

//     int call_number;
//     int fd;
//     uint8_t* filename_str="rtc";
//     printf("Running open system call now\n");
//     // if not initialized, rtc_open (sanity check)
//     call_number = 5; //set sys call number to open
//     asm volatile("movl %1, %%eax ;\ movl %2, %%ebx ;\ movl $0, %%ecx ;\ movl $0, %%edx ;\ int $0x80 ;\  movl %%eax, %0"
//         : "=a"(fd)
//         : "a" (call_number), "b" (filename_str));
//     printf("Open system call complete\n");
//     printf("fd from open: %d\n", fd);
//     if (fd == -1) return FAIL;
//     uint32_t i; // loop counter for diff frequencies
//     int j; // loop counter for printing char
//     int j_cond; // variable end condition for printing char
//     printf("Starting frequency tests. RTC will be set to each frequency for 4 seconds.\n");
//     for(i=2; i<=1024; ){ //max frequency allowed is 1024 Hz
//         // clear();
//         int i_success;
//         printf("tests.c: RTC write call\n");
//         asm volatile("movl %1, %%eax ;\ movl %2, %%ebx ;\ movl %3, %%ecx ;\ movl %4, %%edx ;\ int $0x80 ;\ movl %%eax, %0"
//             : "=r" (i_success)
//             : "a" (4), "b" (fd), "c" (i), "d" (4));
//         if (i_success == -1) return FAIL;
//         // rtc_write((uint8_t*)i, 4); // change frequency to current loop counter
//         printf("RTC frequency = %u Hz\n", i);
//         j_cond = 4*i; // sets end condition such that rtc remains at i Hz for 4 seconds
//         for (j = 0; j < j_cond; j++){
//             // rtc_read(); // wait for rtc interrupt

//             asm volatile("movl %1, %%eax ;\ movl %2, %%ebx ;\ movl %3, %%ecx ;\ movl %4, %%edx ;\ int $0x80 ;\ movl %%eax, %0"
//             : "=r" (i_success)
//             : "a" (3), "b" (fd), "c" (i), "d" (4));
//             if (i_success == -1) return FAIL;

//             printf("1"); // print char to show rtc interrupt complete
//         }
//         printf("\n");
//         i = i * 2; // get next possible frequency (always power of 2)
//     }
//     return PASS;
// }

/* Checkpoint 3 tests */


int generic_system_call(){

    int call_number = 0;
    int firstArg = 0;
    int secondArg = 0;
    int thirdArg = 0;
    int sys_call_ret;
    
    asm volatile ("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            movl %3, %%ecx      \n\
            movl %4, %%edx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
            : "=r" (sys_call_ret)
            : "a" (call_number), "b" (firstArg), "c" (secondArg), "d" (thirdArg)
    );
    
    return PASS;

}

/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// launch your tests here

	//Checkpoint 1 Tests
    clear();
	//TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("division_zero_test", division_zero_test());
	// TEST_OUTPUT("system_call_test", system_call_test());
	//TEST_OUTPUT("paging_test1", paging_test1());
	//TEST_OUTPUT("paging_test2", paging_test2());
	//TEST_OUTPUT("paging_test3", paging_test3());
	//TEST_OUTPUT("paging_test4", paging_test4());
	//TEST_OUTPUT("paging_test5", paging_test5());
	//TEST_OUTPUT("paging_test6", paging_test6());
	// TEST_OUTPUT("paging_test7", paging_test7());
	// TEST_OUTPUT("paging_test8", paging_test8());

	// Checkpoint 2 Tests

	// File system Tests
	// TEST_OUTPUT("test_read_data", test_read_data_basic());	// reading random junk data to just see if something is printing
	// TEST_OUTPUT("test_read_dentry_by_name", test_read_dentry_by_name());

	// File Tests
	// TEST_OUTPUT("test_file_open", test_file_open());
	// TEST_OUTPUT("test_file_close_valid_fd", test_file_close_valid_fd());
	// TEST_OUTPUT("testing_file_read_textfile", test_file_read_textfile());
	// TEST_OUTPUT("testing_file_read_executablefile", test_file_read_executablefile());
	// TEST_OUTPUT("testing_file_read_largefile", test_file_read_largefile());
	// TEST_OUTPUT("testing_file_write", test_file_write());

	// Directory Tests
	// TEST_OUTPUT("test_directory_open", test_directory_open());
	// TEST_OUTPUT("test_directory_close_valid_fd", test_directory_close_valid_fd());
	// TEST_OUTPUT("test_directory_close_invalid_fd", test_directory_close_invalid_fd());
	// TEST_OUTPUT("testing_directory_read", test_directory_read());
	// TEST_OUTPUT("testing_directory_write", test_directory_write());

	// keyboard_test();

    // int i;
    // int i_end;
    // i_end = 5;
    // rtc_open();
    // for (i = 0; i < i_end; i++) rtc_read();
    // clear();
    // TEST_OUTPUT("testing_directory_read", test_directory_read());
    // for (i = 0; i < i_end; i++) rtc_read();
    // clear();
    // TEST_OUTPUT("testing_file_read_textfile", test_file_read_textfile());
    // for (i = 0; i < i_end; i++) rtc_read();
    // clear();
    // TEST_OUTPUT("RTC Frequency Test", rtc_freq_test());

    // TEST_OUTPUT("RTC Write Test", rtc_write_test());

    // TEST_OUTPUT("RTC Close Test", rtc_close_test());

    // TEST_OUTPUT("System Call Test (RTC)", rtc_freq_test_generic_sys_call());

	
}
