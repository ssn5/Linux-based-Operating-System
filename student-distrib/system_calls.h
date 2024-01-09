#include "scheduler.h"
#include "lib.h"
#include "filesystem.h"
#include "terminal.h"
#include "handlers.h"
#include "rtc.h"
#include "x86_desc.h"
#include "paging.h"

#if !defined(SYSTEM_CALLS_H)
#define SYSTEM_CALLS_H

#define EIGHT_MB 0x800000
#define EIGHT_KB 0x2000
#define ADDR_OFFSET 0x4
#define ONETWENTYEIGHT_MB 0x8000000
#define ONETHIRTYTWO_MB 0x8400000


extern int32_t halt(uint8_t status);
extern int32_t jumpTable();
extern int32_t execute(const uint8_t* command);
extern int32_t read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t open(const uint8_t* filename);
extern int32_t close(int32_t fd);
extern int32_t getargs(uint8_t* buf, int32_t nbytes);
extern int32_t vidmap(uint8_t** screen_start);
extern int32_t set_handler(int32_t signum, void* handler_address);
extern int32_t sigreturn(void);

extern int32_t get_global_pid();
extern int32_t get_free_pid();

// extern void hardcodepcb();

typedef struct __attribute__((packed)) helper_struct
{
    int32_t (*open)(const uint8_t* filename);
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*close)(int32_t fd);
} helper_t;

typedef struct __attribute__((packed)) fd_struct         
{
    //jump table pointer
    helper_t file_op_table; 
    uint32_t inode;
    uint32_t fpos;
    uint8_t filetype;
    uint32_t flags;
} fd_t;

typedef struct __attribute__((packed)) pcb_struct         
{
    fd_t fd_array[8];
    uint32_t parent_pcb;
    uint32_t pid;
    uint32_t parent_ebp;
    uint32_t parent_esp;
    uint8_t* args;
    uint32_t parent_esp0;
    uint16_t parent_ss0;
    //uint32_t terminal_id;
    int32_t parent_pid;
} pcb_t;

pcb_t* pcb_ptr;
extern uint32_t global_pid;

extern uint8_t pid_num[6];


#endif
