#include "terminal.h"
#include "system_calls.h"

// #define TERM1_BUFFER 0xB9000
// #define TERM2_BUFFER 0xBA000
// #define TERM3_BUFFER 0xBB000
// #define FOUR_KB 0x1000
// #define VIDEO 0xB8000


volatile uint32_t terminal_id;
int8_t terminal_switch_flag = 0;
terminal_t terminal_arr[3];

/*
 * 	terminal_open
 *   DESCRIPTION: Open the terminal system call
 *   INPUTS: unsigned char* buf, int32_t count
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t terminal_open(const uint8_t* filename){
    return 0;
}

/*
 * 	terminal_close
 *   DESCRIPTION: Close the terminal system call
 *   INPUTS: unsigned char* buf, int32_t count
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t terminal_close(int32_t fd){
    return 0;
}

/*
 * 	terminal_read
 *   DESCRIPTION: Read keyboard buffer and copy it to user buffer
 *   INPUTS: unsigned char* buf, int32_t count
 *   OUTPUTS: none
 *   RETURN VALUE: number of bytes copied
 *   SIDE EFFECTS: keyboard buffer gets cleared after copy
 */
int32_t terminal_read(int32_t fd, void* buf_arg, int32_t count){
    //check if number of bytes to be read between 0 and 128 or buf == NULL, return -1
    if (count > 128){
        count = 128;
    }
    uint8_t* buf = (uint8_t*)buf_arg;
    if (buf == NULL || count < 0 || count > 128){return -1;}
    unsigned int num, final_count;

    //run in an infinite loop until a newline character printed
    while (line_buffer[line_buf_index] != '\n'){}

    //critical section
    cli();
    //check if the line buffer index is less than count (if so, only read number of bytes as typed) 
    if (line_buf_index < count){
        final_count = line_buf_index;
    }
    else{
        final_count = count;
    }

    //if count <= terminal_arr[terminal_id].terminal_buf_index, add a newline manually at the end
    if (count <= line_buf_index){
        for (num = 0; num < final_count; num++){
            buf[num] = line_buffer[num]; 
        }
        buf[num] = '\n';
    }
    //copy all the bytes from line buffer to user buffer
    else{
        //add extra count to final to account for newline character
        for (num = 0; num < final_count + 1; num++){
            buf[num] = line_buffer[num]; 
        }
        final_count++;
    }
        
    // clear line buffer by setting each character to NULL
    for (num = 0; num < line_buf_index + 1; num++){ //add extra count to final count to account for newline character
        line_buffer[num] = NULL;
    }

    sti();

    //reset index to 0
    line_buf_index = 0;

    return final_count;
}

/*
 * 	terminal_write
 *   DESCRIPTION: Print user buffer to the screen
 *   INPUTS: unsigned char* buf, int32_t count
 *   OUTPUTS: User buffer characters
 *   RETURN VALUE: Number of characters printed
 *   SIDE EFFECTS: User buffer gets cleared
 */
int32_t terminal_write(int32_t fd, const void* buf_arg, int32_t count){
    uint8_t* buf = (uint8_t*) buf_arg;
    //return -1 if bufffer == 1 or no bytes copied
    if (buf == NULL || count == 0){return -1;}
    unsigned int num;
    //print each character out
    for (num = 0; num < count; num++){
        if(buf[num] != NULL){
            putc(buf[num]);
        }
    }

    
    //return number of bytes written
    return count;
}

/* terminal_init
* INPUTS: none
* OUTPUTS: none
* RETURN: none
* DESCRIPTION: initializes terminal struct
*/
void terminal_init(void){
    terminal_id = 0;
    int i = 0;
    for(i = 0; i < 3; i++){         // initializing fields of terminal structs for all terminals
        terminal_arr[i].ebp_saved = 0;
        terminal_arr[i].esp_saved = 0;
        terminal_arr[i].terminal_buf[0] = (uint8_t)'\0';
        terminal_arr[i].terminal_buf_index = 0;

        terminal_arr[i].key_curr = -1;
        terminal_arr[i].cursor_xpos = 0;
        terminal_arr[i].cursor_ypos = 0;
        
        terminal_arr[i].curr_pcb = NULL;
        terminal_arr[i].curr_pid = -1;
        terminal_arr[i].curr_rtc = -1;

    }
    // assinging video memory locations for terminals
    terminal_arr[0].vmem_location = (uint32_t)TERM1_BUFFER;
    terminal_arr[1].vmem_location = (uint32_t)TERM2_BUFFER;
    terminal_arr[2].vmem_location = (uint32_t)TERM3_BUFFER;

    return;
}

/* terminal_switch
* INPUTS: new_terminal
* OUTPUTS: none
* RETURN: none
* DESCRIPTION: used for terminal switching
*/
void terminal_switch(uint32_t new_terminal){

    cli();

    if (new_terminal == terminal_id){       //  no need to switch to the same terminal
        return;
    }

    disable_page(terminal_arr[terminal_id].vmem_location);      // disabling paging

    memcpy((uint8_t*)(VIDEO + (FOUR_KB * (terminal_id+1))), (uint8_t*)(VIDEO), FOUR_KB);        // copying from vmem to terminal buffer
    memcpy((uint8_t*)(VIDEO), (uint8_t*)(VIDEO + (FOUR_KB * (new_terminal+1))), FOUR_KB);       // copying from terminal buffer to vmem

    
    memcpy((uint8_t*)(terminal_arr[terminal_id].terminal_buf), (uint8_t*)(line_buffer), 128);      // copying over from global line buffer to corresponding terminal buffer
    terminal_arr[terminal_id].terminal_buf_index = line_buf_index;
    memcpy((uint8_t*)(line_buffer), (uint8_t*)(terminal_arr[new_terminal].terminal_buf), 128);
    line_buf_index = terminal_arr[new_terminal].terminal_buf_index;

    enable_page(terminal_arr[new_terminal].vmem_location);      // enabling paging for the new terminal's video memory

    // updating cursor positions and saving previous terminal's cursor
    int* pos = get_screen_coords();
    terminal_arr[terminal_id].cursor_xpos = pos[0];
    terminal_arr[terminal_id].cursor_ypos = pos[1];

    change_cursor(terminal_arr[new_terminal].cursor_xpos, terminal_arr[new_terminal].cursor_ypos);

       if((terminal_arr[new_terminal].curr_pcb == NULL)){       // dynamically initializing shells in new terminals if they dont have a base shell
        send_eoi(1);
        sti();
        terminal_id = new_terminal;
        terminal_switch_flag = 1;
        running_terminal = new_terminal;
        execute((uint8_t*)"shell");
    }

    if (terminal_arr[new_terminal].curr_pcb != NULL){
        pcb_ptr = terminal_arr[new_terminal].curr_pcb;
    }

    terminal_id = new_terminal;
    running_terminal = new_terminal;        // updating running terminal

    send_eoi(1);        // sending eoi signal
    sti();

    return;

}

