#if !defined(PAGING_H)
#define PAGING_H
#include "types.h"

extern void initialize_paging();
extern void loadPageDirectory(unsigned int*);
extern void enablePaging();
extern void flush_tlb(uint32_t curr_pid);

extern void executable_page();
extern void disable_page(uint32_t vmem_loc);
extern void enable_page(uint32_t vmem_loc);
extern void schedule_visible_page();
extern void schedule_invisible_page(uint32_t vmem_loc);



//have taken number of bits for each parameter in structs from IA-32 Manual (Intel Manual Vol 3)

// struct for 4kb page directory entry
typedef struct __attribute__ ((packed, aligned(4))) vid_struct{ //aligned on a 4 byte boundary

            uint8_t present         : 1 ;   // 1 bit parameter
            uint8_t rw              : 1 ;   // 1 bit parameter
            uint8_t us              : 1 ;   // 1 bit parameter
            uint8_t pwt             : 1 ;   // 1 bit parameter
            uint8_t pcd             : 1 ;   // 1 bit parameter
            uint8_t a               : 1 ;   // 1 bit parameter
            uint8_t reserved        : 1 ;   // 1 bit parameter
            uint8_t ps              : 1 ;   // 1 bit parameter
            uint8_t g               : 1 ;   // 1 bit parameter
            uint8_t avail           : 3 ;   // 3 bit parameter
            uint32_t table_base_add : 20;   // 20 bit parameter

        } vid_struct_t;


// struct for 4mb page directory entry
typedef struct __attribute__ ((packed, aligned(4))) kernel_struct{  //aligned on a 4 byte boundary

            uint8_t present         : 1 ;   // 1 bit parameter
            uint8_t rw              : 1 ;   // 1 bit parameter
            uint8_t us              : 1 ;   // 1 bit parameter
            uint8_t pwt             : 1 ;   // 1 bit parameter  
            uint8_t pcd             : 1 ;   // 1 bit parameter
            uint8_t a               : 1 ;   // 1 bit parameter
            uint8_t d               : 1 ;   // 1 bit parameter
            uint8_t ps              : 1 ;   // 1 bit parameter
            uint8_t g               : 1 ;   // 1 bit parameter
            uint8_t avail           : 3 ;   // 3 bit parameter
            uint8_t pat             : 1 ;   // 1 bit parameter
            uint16_t reserved       : 9 ;   // 9 bit parameter
            uint16_t page_base_add  : 10;   // 10 bit parameter

        } kernel_struct_t;

//union for both page directory types (4kb & 4mb)
typedef union page_dir_entry {
    vid_struct_t page_dir_vid;
    kernel_struct_t page_dir_kernel;
} page_dir_entry_t;

//struct for page table entry
typedef struct __attribute__ ((packed, aligned(4))) page_table_entry {  //aligned on a 4 byte boundary

    uint8_t present         : 1 ;   // 1 bit parameter
    uint8_t rw              : 1 ;   // 1 bit parameter
    uint8_t us              : 1 ;   // 1 bit parameter
    uint8_t pwt             : 1 ;   // 1 bit parameter
    uint8_t pcd             : 1 ;   // 1 bit parameter
    uint8_t a               : 1 ;   // 1 bit parameter
    uint8_t d               : 1 ;   // 1 bit parameter
    uint8_t pat             : 1 ;   // 1 bit parameter
    uint8_t g               : 1 ;   // 1 bit parameter
    uint8_t avail           : 3 ;   // 3 bit parameter
    uint32_t page_base_add  : 20;   // 20 bit parameter

} page_table_entry_t;

//page directory initialization
page_dir_entry_t pde[1024] __attribute__((aligned(4096))); //1024 entries in the directory, aligned by 4kb (4096 bytes)

//page table initialization
page_table_entry_t pte[1024] __attribute__((aligned(4096)));  //1024 entries in the directory, aligned by 4kb (4096 bytes)

page_table_entry_t vidmem_pte[1024] __attribute__((aligned(4096)));  //1024 entries in the directory, aligned by 4kb (4096 bytes)

extern void vidmem_assign(int vid_addr);

#endif
