#include "keyboard.h"

uint8_t flags[4];   // shift, ctrl, alt, capslock flags (key press=1, key release= 0)
uint8_t boot_flag[2] = {0, 0};

/*
 * 	keyboard_init
 *   DESCRIPTION: Initializes the keyboard by unmasking the corresponding keyboard init line on the *master pic
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: enables the IRQ 1 line on the master PIC
 */

void keyboard_init(){
    enable_irq(1); //enable interrupts from irq1 keyboard
    line_buf_index = 0;
    //set all flags to 0
    unsigned int i;
    for (i = 0; i < 4; i++){
        flags[i] = 0;
    }
}

/*
 * 	keyboard_handler
 *   DESCRIPTION: Execute the code in this handler whenever keyboard raises an interrupt.
 *   INPUTS: none
 *   OUTPUTS: Prints the characters obtained by mapping the scancodes, obtained from the keyboard *data port
 *   RETURN VALUE: none
 *   SIDE EFFECTS: clears and sets the interrupt flag, while executing the code inside the handler
 */ 

void keyboard_handler(){
    unsigned char returnChar;      // stores character to print
    uint8_t scancode;              // stores scancode received from keyboard

    //array to store each ASCII character based on the scancode index (a 0 indicates no corresponding character to be printed)
    unsigned char ascii_code[58] = {0, 0, '1', '2', '3', '4', '5', '6', '7', '8', 
                                  '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r',
                                  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
                                  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
                                  '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n',
                                  'm', ',', '.', '/', 0, 0, 0, ' '};

    //array to store each shifted ASCII character based on the scancode index (a 0 indicates no corresponding character to be printed)
    unsigned char ascii_code_shift[58] = {0, 0, '!', '@', '#', '$', '%', '^', '&', '*', 
                                         '(', ')', '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R',
                                         'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
                                         'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
                                         '\"', '~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N',
                                         'M', '<', '>', '?', 0, 0, 0, ' '};

    cli();                         // start critical section
    scancode = inb(0x60);          // get scan code from data port on keyboard

    //right or left shift pressed
    if (scancode == 0x2A || scancode == 0x36){
        flags[0] = 1;
    }

    //right or left shift released
    else if (scancode == 0xAA || scancode == 0xB6){
        flags[0] = 0;
    }

    //right or left control pressed
    else if (scancode == 0x1D){
        flags[1] = 1;
    }

    //right or left control released
    else if (scancode == 0x9D){
        flags[1] = 0;
    }

    // left-alt pressed
    else if (scancode == 0x38){
        flags[2] = 1;
    }

    // left-alt pressed
    else if (scancode == 0xB8){
        flags[2] = 0;
    }

    //caps lock pressed
    else if (scancode == 0x3A){
        flags[3] = flags[3] ^ 0x1; //xor with 0x1 since, each time we press the key we want to change the state of the flag
    }

    //if ctrl-l pressed, clear the screen
    else if (flags[1] == 1 && (scancode == 38)){
        clear();
    }

    // alt + F1
    else if (flags[2] == 1 && (scancode == 0x3B)){
        terminal_switch(0);
    }

    // alt + F2
    else if (flags[2] == 1 && (scancode == 0x3C)){
        terminal_switch(1);
    }

    // alt + F3
    else if (flags[2] == 1 && (scancode == 0x3D)){
        terminal_switch(2);
    }

    //backspace character
    else if (scancode == 14){
        //check to not backspace previous terminal entry
        if (line_buf_index > 0){
            putc(ascii_code[14]);
            (line_buf_index)--;   //decrement index since, removing a character
        }
    }

    //enter character
    else if (scancode == 28){
        //check since max number of characters to be typed is 128 (128 since last character should be enter)
        if (line_buf_index < 128){
            putc(ascii_code[28]);
            line_buffer[line_buf_index] = ascii_code[28]; 
        }
    }

    //alphabet characters (A-Z), the characters reside within these scancodes
    else if ((scancode >= 16 && scancode <= 25) || (scancode >= 30 && scancode <= 38) || (scancode >= 44 && scancode <= 50)){
        //check if shift and caps lock are opposite states (if so, they don't cancel each others effects)
        if ((flags[0] == 1 && flags[3] == 0) || (flags[0] == 0 && flags[3] == 1)){
            returnChar = ascii_code_shift[scancode];
        }
        //
        else{
            returnChar = ascii_code[scancode];
        }
        //add character to the line buffer if line buffer max not reached (127 since want the last character to be enter)
        if (line_buf_index < 127){
            putc(returnChar);
            line_buffer[line_buf_index] = returnChar;
            (line_buf_index)++;
        }
    }
    
    //tab character
    else if (scancode == 15){
        // check if index < 124 since tab is 4 spaces (128-4=124)
        if ((line_buf_index) < 124){
            putc(ascii_code[15]);
            unsigned int j;
            // add a space 4 times
            for (j = 0; j < 4; j++){
                line_buffer[line_buf_index] = ' ';
                (line_buf_index)++;
            }    
        }
    }

    //any remaining characters that don't come under the above cases
    else if (scancode <= 57){
        //if shift is pressed
        if (flags[0] == 1){
            returnChar = ascii_code_shift[scancode];
        }
        else {
            returnChar = ascii_code[scancode];  
            }

        //add character to the line buffer if line buffer max not reached (127 since want the last character to be enter)
        if (line_buf_index < 127){
            putc(returnChar);
            line_buffer[line_buf_index] = returnChar;
            (line_buf_index)++;
        }
    }
        
    send_eoi(1);  // end or interrupt for irq1 keyboard
    sti();        // end critical section
}
