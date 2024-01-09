#include "system_calls.h"



uint32_t previous_ebp;
uint32_t previous_esp;
uint32_t user_eip;

uint32_t global_pid = 0;
uint8_t pid_num[6] = {0, 0, 0, 0, 0, 0};

/* get_free_pid
* INPUTS: none
* OUTPUTS: none
* RETURN: get the next free pid
* DESCRIPTION: iterate through the array of pid indices and find the next free pid
*/
extern int32_t get_free_pid(){
    int i = 0;
    for(i = 0; i < 6; i++){
        if(pid_num[i] == 0){
            return i;
        }
    }
    return -1;
}

/* get_global_pid
* INPUTS: none
* OUTPUTS: none
* RETURN: global_pid
* DESCRIPTION: getter function for global pid
*/
extern int32_t get_global_pid(){
    return global_pid;
}

/* halt
* INPUTS: none
* OUTPUTS: none
* RETURN: status of halting task
* DESCRIPTION: responsible for halting and returning
*/
int32_t halt(uint8_t status){

    cli();
    pcb_t* curr_pcb = terminal_arr[terminal_id].curr_pcb;
    int temp_pid = curr_pcb->pid;
    uint32_t curr_ebp, curr_esp;


    if (temp_pid < 0){    // should never be executed but sanity check
        return 0;
    } 

    if (curr_pcb->parent_pid < 0){ // if trying to exit base shell
        printf("\n Can't exit base shell. Restarting shell.\n\n");
        terminal_arr[running_terminal].curr_pid = -1;
        pid_num[temp_pid] = 0;
        execute((uint8_t *)"shell"); // restart shell
    }

    pid_num[temp_pid] = 0;

    global_pid = pcb_ptr->parent_pid;

    pcb_ptr = (pcb_t*)(curr_pcb)->parent_pcb;
    curr_ebp = curr_pcb->parent_ebp;
    curr_esp = curr_pcb->parent_esp;

    
    // set tss.esp0 and tss.ss0 for task switch
    tss.esp0 = pcb_ptr->parent_esp0;
    tss.ss0 = pcb_ptr->parent_ss0;

    terminal_arr[terminal_id].curr_pcb = pcb_ptr;
    terminal_arr[terminal_id].curr_pid = global_pid;
    terminal_arr[terminal_id].esp0_term = tss.esp0;
    terminal_arr[terminal_id].ss0_term = tss.ss0;
    terminal_arr[terminal_id].ebp_saved = curr_ebp;
    terminal_arr[terminal_id].esp_saved = curr_esp;

    // flush tlb to restore paging of previous program
    flush_tlb(global_pid);

    int i;
    // close any open files
    for (i = 2; i < 8; i++)
    {
        if (curr_pcb->fd_array[i].flags == 1)
        {
            if (curr_pcb->fd_array[i].file_op_table.close != NULL){
                curr_pcb->fd_array[i].file_op_table.close(i);
            }
            curr_pcb->fd_array[i].file_op_table.read = NULL;
            curr_pcb->fd_array[i].file_op_table.write = NULL;
            curr_pcb->fd_array[i].file_op_table.open = NULL;
            curr_pcb->fd_array[i].file_op_table.close = NULL;
            curr_pcb->fd_array[i].inode = 0;
            curr_pcb->fd_array[i].fpos = 0;
            curr_pcb->fd_array[i].flags = 0;
            curr_pcb->fd_array[i].filetype = 0;
        }
    }

    sti();
    // store ebp value and status (return val) to eax
    // return to parent program
    asm volatile("            \n\
            movl %0, %%ebx      \n\
            movl %2, %%esp      \n\
            xorl %%eax, %%eax   \n\
            movb %1, %%al      \n\
            movl %%ebx, %%ebp   \n\
            leave              \n\
            ret                \n\
            "
                 :
                 : "g"(curr_ebp), "g"(status), "g"(curr_esp)
                 : "cc");

    return 0;
}

/* jumpTable
* INPUTS: none
* OUTPUTS: none
* RETURN: 0
* DESCRIPTION: helper for jumptable
*/
int32_t jumpTable(){
    return 0;
}

/* execute
* INPUTS: command
* OUTPUTS: none
* RETURN: -1 if command can't be executed, 0-255 based on call's halt
* DESCRIPTION: attempts to load and execute a new program, handing off the processor to the new program
*/
int32_t execute(const uint8_t* command){

    uint32_t val;


    if((terminal_switch_flag == 1) && (terminal_arr[terminal_id].curr_pid == -1)){
        running_terminal = terminal_id;
    }
    else{
        while (running_terminal != terminal_id){}
    }
    terminal_switch_flag = 0;
    

    cli();

    

    // paramter validation
    if(command == NULL){ 
        sti();
        return -1;
    }

    //intializing the local variable
    uint32_t i = 0;
    uint32_t size = strlen((int8_t*)command);
    uint8_t file_name[size];
    uint8_t local_name[size];
    int flag = 0;
    dentry_t temp;
    int8_t* temp1;
    int index = 0;
    uint32_t old_ebp;
    uint32_t old_esp;


    // local name variable holds the command input on the terminal to be passed down to getargs
    while(i < size){
        local_name[i] = command[i];
        i++;
    }

    //terminate the string with a null charcater
    local_name[i]='\0';

    i = 0;

    // file name variables used to executable
    while ((i < size) && (command[i] != 0x20)) {
        file_name[i] = command[i];
        i++;
    }
    file_name[i] = '\0';
    
    for(i = 0; i < bootblock_ptr->directory_num; i++){     // iterating through all directory entry files
        temp = dentry_ptr[i];
        temp1 = (int8_t*)(temp.filename);
        if(strncmp((int8_t*)file_name, temp1, 32) == 0){        // checking if file names are equivalent
            flag = 1;
            index = i;
            break;
        }
    }

    if (flag == 0){
        sti();
        return -1;
    }

    int temp_pid = get_free_pid();
        if(temp_pid == -1){
            puts((int8_t*)"Can't run more than 6 processes");
            sti();
            return -1;
        }
        else{
            global_pid = temp_pid;
            pid_num[temp_pid] = 1;
        }


    // getting exact file size of file to be loaded
    size = (inode_ptr + dentry_ptr[index].inode)->length;

    uint8_t buf[4];

	// loading file contents into buf
    if (read_data(dentry_ptr[index].inode, 0, buf, 4) == -1) {  
        sti();
		return -1;
	}

    // checking for magic constant to see if it is an executable
    if (!((buf[0]==0x7F) && (buf[1]==0x45) && (buf[2]==0x4C) && (buf[3]==0x46))) {     
        sti();
        return -1;
    } 

    //initialize page
    executable_page();    

    // loading file contents into buf
    if (read_data(dentry_ptr[index].inode, 0, (uint8_t*)0x08048000, size) == -1) {  
        sti();
		return -1;
	}

    //intializing a process control block
    pcb_ptr = (pcb_t*)(EIGHT_MB - (EIGHT_KB * (global_pid + 1)));
    pcb_ptr->parent_pcb = (uint32_t) terminal_arr[terminal_id].curr_pcb;

    pcb_ptr->parent_pid = terminal_arr[terminal_id].curr_pid;
    
    terminal_arr[terminal_id].curr_pid = global_pid;
    terminal_arr[terminal_id].curr_pcb = pcb_ptr;

    // initializing entry for stdin (fd0)
    pcb_ptr->fd_array[0].file_op_table.read = terminal_read;
    pcb_ptr->fd_array[0].inode = 0;
    pcb_ptr->fd_array[0].fpos = 0;
    pcb_ptr->fd_array[0].flags = 1;

    // initializing entry for stdout (fd1)
    pcb_ptr->fd_array[1].file_op_table.write = terminal_write;
    pcb_ptr->fd_array[1].inode = 0;
    pcb_ptr->fd_array[1].fpos = 0;
    pcb_ptr->fd_array[1].flags = 1;

    pcb_ptr->pid = global_pid;
    pcb_ptr->args = local_name;

    //restoring old ebp
    asm volatile ("             \n\
            movl %%ebp, %0      \n\
            movl %%esp, %1      \n\
            "
            : "=g"(old_ebp), "=g"(old_esp)
            :  
    );

    pcb_ptr->parent_ebp = old_ebp;
    pcb_ptr->parent_esp = old_esp;


    // initializing entries for fd 2-7 (i.e all except stdin and stdout) to NULL
    for (i = 2; i < 8; i++){
        pcb_ptr->fd_array[i].file_op_table.read = NULL;
        pcb_ptr->fd_array[i].file_op_table.write = NULL;
        pcb_ptr->fd_array[i].file_op_table.open = NULL;
        pcb_ptr->fd_array[i].file_op_table.close = NULL;
        pcb_ptr->fd_array[i].inode = 0;
        pcb_ptr->fd_array[i].fpos = 0;
        pcb_ptr->fd_array[i].flags = 0;
    }

    // loading file contents into buf
    if (read_data(dentry_ptr[index].inode, 24, buf, 4) == -1) {  
        sti();
		return -1;
	}

    // starting address of first instructions to be executed (given in doc)
    user_eip = (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
    
    // setting fields to tss to switch to the kernel stack
    tss.esp0 = EIGHT_MB - (EIGHT_KB * (global_pid)) - 4;
    tss.ss0 = KERNEL_DS;
    terminal_arr[terminal_id].esp0_term = tss.esp0;
    terminal_arr[terminal_id].ss0_term = tss.ss0;

    val = pcb_ptr->parent_pcb;
    pcb_ptr->parent_esp0 = tss.esp0;
    pcb_ptr->parent_ss0 = tss.ss0;


    // inline assembly to push iret context to stack
    sti();

    //$0x083FFFFC: (132MB-4Bytes): User program ESP
    asm volatile ("                 \n\
            pushl   %0              \n\
            movl $0x083FFFFC, %%esi \n\
            pushl %%esi             \n\
            pushfl                  \n\
            pushl   %1              \n\
            pushl   %2              \n\
            iret                    \n\
            "
            :
            : "g"(USER_DS),"g"(USER_CS),"g"(user_eip)
            : "memory"
    );

    // FAKE_IRET(user_eip);

    return 0;
}


/* read
* INPUTS: fd, buf, nbytes
* OUTPUTS: none
* RETURN: number of bytes read
* DESCRIPTION: read data from keyboard, RTC, or directory
*/
int32_t read(int32_t fd, void* buf, int32_t nbytes){
    // printf("system_calls.c: System Call Read\n");

    pcb_t* curr_pcb = terminal_arr[terminal_id].curr_pcb;

    if (nbytes == 0){ 
        return 0;
    }

    if(fd < 0 || fd > 7 || buf == NULL || nbytes <= 0){      // parameter validation
        return -1;
    }

    if(fd == 0){        // stdin case
        return terminal_read(fd, buf, nbytes);
    }

    if(fd == 1){        // nothing to be done for stdout
        return -1;
    }

    if(pcb_ptr->fd_array[fd].flags == 0){       // if not present return -1
    return -1;
    }
    
    return ((curr_pcb->fd_array[fd]).file_op_table.read(fd, buf, nbytes));
}

/* write
* INPUTS: fd, buf, nbytes
* OUTPUTS: none
* RETURN: number of bytes written or -1 on failure
* DESCRIPTION: writes data to the terminal or to a device (RTC)
*/
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    // printf("system_calls.c: System Call Write\n");
    pcb_t* curr_pcb = terminal_arr[terminal_id].curr_pcb;

    if (nbytes == 0) {
        return 0;
    }

    if(fd < 0 || fd > 7 || buf == NULL || nbytes <= 0){      // parameter validation
        return -1;
    }

    if(fd == 0){        // nothing to be done for stdin
        return -1;
    }

    if(fd == 1){       // stdout case
        return terminal_write(fd, buf, nbytes);
    }

    if(pcb_ptr->fd_array[fd].flags == 0){       // if not present return -1
    return -1;
    }

    return ((curr_pcb->fd_array[fd]).file_op_table.write(fd, buf, nbytes));    
}

/* open
* INPUTS: filename
* OUTPUTS: none
* RETURN: number of bytes written or -1 on failure
* DESCRIPTION: provides access to file system by finding directory entry corresponding to filename
*/
int32_t open(const uint8_t* filename){
    // printf("System Call Open\n");
    int temp_flag = 0;
    int fd = 0;
    int i = 0;

    if(filename == NULL){ //parameter validation
        return -1;
    }

    pcb_t* curr_pcb = terminal_arr[terminal_id].curr_pcb;
    dentry_t temp_dentry;
    if(read_dentry_by_name(filename, (&temp_dentry)) == -1){
        return -1;
    }

    for(i = 2; i < 8; i++){ //finding first available entry in fd table
        if(pcb_ptr->fd_array[i].flags == 0){
            fd = i;
            temp_flag = 1;
            break;
        }
    }
    if(temp_flag == 0){ // if no entry in fd table available, return error
        return -1; 
    }

    switch(temp_dentry.filetype){
        case 0:     // RTC file
        (curr_pcb->fd_array[fd]).file_op_table.read = rtc_read;
        (curr_pcb->fd_array[fd]).file_op_table.write = rtc_write;
        (curr_pcb->fd_array[fd]).file_op_table.open = rtc_open;
        (curr_pcb->fd_array[fd]).file_op_table.close = rtc_close;
        (curr_pcb->fd_array[fd]).fpos = -1;
        break;

        case 1:     // Directory
        (curr_pcb->fd_array[fd]).file_op_table.read = directory_read;
        (curr_pcb->fd_array[fd]).file_op_table.write = directory_write;
        (curr_pcb->fd_array[fd]).file_op_table.open = directory_open;
        (curr_pcb->fd_array[fd]).file_op_table.close = directory_close;
        (curr_pcb->fd_array[fd]).fpos = 0;
        break;

        case 2:     // Regular file
        (curr_pcb->fd_array[fd]).file_op_table.read = file_read;
        (curr_pcb->fd_array[fd]).file_op_table.write = file_write;
        (curr_pcb->fd_array[fd]).file_op_table.open = file_open;
        (curr_pcb->fd_array[fd]).file_op_table.close = file_close;
        (curr_pcb->fd_array[fd]).fpos = 0;
        break;

        default:
        return -1;
    }
    // setting other fields for current fd (inode, flags, filetype)
    (curr_pcb->fd_array[fd]).inode = temp_dentry.inode;
    (curr_pcb->fd_array[fd]).flags = 1;
    (curr_pcb->fd_array[fd]).filetype = temp_dentry.filetype;

    // checking if file can be opened or not
    if((curr_pcb->fd_array[fd]).file_op_table.open(filename) < 0){
        return -1;
    }

    return fd;
}

/* close
* INPUTS: fd
* OUTPUTS: none
* RETURN: 0 on success or -1 on failure
* DESCRIPTION: closes the specified file descriptor and makes it available for later calls to open
*/
int32_t close(int32_t fd){
    // printf("System Call Close\n");
    pcb_t* curr_pcb = terminal_arr[terminal_id].curr_pcb;
    
    // parameter validation
    if (fd < 2 || fd > 7) {
        return -1;
    }

    // parameter validation 
    if(fd < 0 || fd > 7 || 
    (curr_pcb->fd_array[fd]).flags == 0 || 
    (curr_pcb->fd_array[fd]).file_op_table.close(fd) < 0 ||
    fd < 2){
        return -1;
    }

    // setting fields for closed fd to 0
    (curr_pcb->fd_array[fd]).file_op_table.read = 0;
    (curr_pcb->fd_array[fd]).file_op_table.write = 0;
    (curr_pcb->fd_array[fd]).file_op_table.open = 0;
    (curr_pcb->fd_array[fd]).file_op_table.close = 0;
    (curr_pcb->fd_array[fd]).inode = 0;
    (curr_pcb->fd_array[fd]).fpos = 0;
    (curr_pcb->fd_array[fd]).filetype = -1;
    (curr_pcb->fd_array[fd]).flags = 0;

    return 0;
}

/* getargs
* INPUTS:  buf, nbytes
* OUTPUTS: none
* RETURN: 0 on success or -1 on failure
* DESCRIPTION: passing argument to function being loaded in execute
*/
int32_t getargs(uint8_t* buf, int32_t nbytes){
    // printf("System Call getargs\n");
    uint32_t i = 0;
    uint32_t counter = 0;
    pcb_t* curr_pcb = terminal_arr[terminal_id].curr_pcb;
    uint8_t *tmp = curr_pcb->args;
    uint32_t size = strlen((int8_t*)tmp);

    // parameter validation
    if(((size + 1) > nbytes) || (size == 0)){
        return -1;
    }

    // removing preceding spaces
    while (tmp[i] == 0x20){
        i++;
    }

    // ignoring executable
    while (tmp[i] != 0x20){
        i++;
    }

    // removing spaces between executable and arg
    while (tmp[i] == 0x20){
        i++;
    }

    if(tmp[i] == NULL){             // checking if no args provided (for cat and grep)
        printf("No arguments provided \n");
        return -1;
    }
    
    // copying args into buf
    while ((tmp[i] != 0x20) && ((i + counter) < size)){
        buf[counter] = tmp[i+counter];
        counter++;
    }
    buf[counter] = '\0';

    return 0;
}

/* vidmap
* INPUTS: screen_start
* OUTPUTS: none
* RETURN: 0 on success or -1 on failure
* DESCRIPTION: mapping video memory
*/
int32_t vidmap(uint8_t** screen_start){

    // parameter validation
    if(screen_start == NULL ||
    (int)screen_start < (int)ONETWENTYEIGHT_MB||
    (int)screen_start > (int)ONETHIRTYTWO_MB){
        return -1;
    }
    // creating 4kB page at 132 MB virtual
    vidmem_assign(terminal_arr[terminal_id].vmem_location);

    *screen_start = (uint8_t*)(terminal_arr[terminal_id].vmem_location); 
    return (int32_t)(*screen_start);
}

/* set_handler
* INPUTS: signum, handler_address
* OUTPUTS: none
* RETURN: 0 on success or -1 on failure
* DESCRIPTION: EC 
*/
int32_t set_handler(int32_t signum, void* handler_address){
    printf("System Call set_handler\n");
    return 0;
}

/* sig_return
* INPUTS: none
* OUTPUTS: none
* RETURN: 0 on success or -1 on failure
* DESCRIPTION: EC 
*/
int32_t sigreturn(void){
    printf("System Call sigreturn\n");
    return 0;
}

