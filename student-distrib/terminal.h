#if !defined(TERMINAL_H)
#define TERMINAL_H
#include "types.h"
#include "paging.h"
#include "keyboard.h"
#include "lib.h"
#include "system_calls.h"



#define TERM1_BUFFER 0xB9000
#define TERM2_BUFFER 0xBA000
#define TERM3_BUFFER 0xBB000
#define FOUR_KB 0x1000

int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);
int32_t terminal_read(int32_t fd, void* buf_arg, int32_t count);
int32_t terminal_write(int32_t fd, const void* buf_arg, int32_t count);

extern int8_t terminal_switch_flag;

// terminal struct
typedef struct __attribute__((packed)) terminal_struct         
{
    int8_t curr_pid;
    uint32_t esp_saved;
    uint32_t ebp_saved;
    uint8_t terminal_buf[128];        // max 128 allowed in line buf
    uint32_t terminal_buf_index; 

    // pcb-related
    pcb_t* curr_pcb;

    // cursor and keys
    int key_curr;
    uint8_t cursor_xpos;
    uint8_t cursor_ypos;

    //vmem
    uint32_t vmem_location;

    uint32_t esp0_term;
    uint16_t ss0_term;

    // rtc
    volatile uint32_t curr_rtc;
} terminal_t; 

extern terminal_t terminal_arr[3];

extern volatile uint32_t terminal_id;

void terminal_init(void);
void terminal_switch(uint32_t new_terminal);

#endif





