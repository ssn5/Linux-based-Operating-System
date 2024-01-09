#include "filesystem.h"


static int16_t read_directory_index;

/* bootblock_init
* DESCRIPTION: initializing all structs and their pointers
* INPUTS: bootblock_address
* OUTPUTS: none
* RETURN: 0 on sucess
*/
int32_t bootblock_init(uint32_t bootblock_address){
    fstart = bootblock_address;
    bootblock_ptr = (bootblock_t*)(fstart);
    dentry_ptr = (dentry_t*)(bootblock_ptr->directory_entries);
    inode_ptr = (inode_t*)(bootblock_ptr + 1);
    datablock_ptr = (datablock_t*)(1 + bootblock_ptr + bootblock_ptr->inodes_num);     // + 1 because structure is zero indexed but bootblock is in first 4kB
    return 0;       // return 0 to indicate initialization success
}


/* read_dentry_by_name
* INPUTS: fname, dentry
* OUTPUTS: none
* RETURN: -1 on failure and 0 on success
* SIDE EFFECTS: given the name, reading corresponding entry from directory
*/
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    int flag = 0;
    int i = 0;
    int index;
    dentry_t temp;
    int8_t* temp1;

    for(i = 0; i < bootblock_ptr->directory_num; i++){     // iterating through all directory entry files
        temp = dentry_ptr[i];
        temp1 = (int8_t*)(temp.filename);
        if(strncmp((int8_t*)fname, temp1, 32) == 0){        // checking if file names are equivalent
            flag = 1;
            index = i;
            break;
        }
    }
    if(flag != 1){
        return -1;      // return -1 on failure
    }

    return read_dentry_by_index(index, dentry);
}


/* read_dentry_by_index
* INPUTS: index, dentry
* OUTPUTS: none
* RETURN: -1 on failure and 0 on success
* SIDE EFFECTS: given index, read corresponding dentry by name 
*/
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
    dentry_t temp;
    if(index < 0 || index > 63 || dentry == NULL){      // parameter validation: checking for invalid index or dentry NULL check
        return -1;      // return -1 on failure
    }

    temp = dentry_ptr[index];
    strncpy(dentry->filename, temp.filename, 32);
    dentry->filetype = temp.filetype;       // allocating filetype
    dentry->inode = temp.inode;             // allocating inode number
    return 0;       // return 0 on success
}

/* read_data
* INPUTS: inode, offset, buf, length
* OUTPUTS: none
* RETURN: -1 if failure (invalid inode), length of file read if success
* SIDE EFFECTS: reading a max of length bytes starting from offset until the end from file with given inode
*/
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){

    inode_t* inode_curr = (inode_t*)(inode_ptr + inode);     // ptr to inode for file we want to read
    int index_curr_datablock = 0;
    datablock_t* datablock_curr;
    uint8_t* data_curr;
    uint32_t bytes_read = 0;

    // parameter validation
    if(inode < 0 || inode > (bootblock_ptr->inodes_num - 1) ||          // inode in valid range or not
    inode_curr->length == 0 ||                                          // inode length 0 then nothing to read
    buf == NULL){                                                       // buf NULL check
        return -1;      // return -1 on failure
    }
    // reinitializing ptrs to all structures
    bootblock_ptr = (bootblock_t*)(fstart);     // address of bootblock
    dentry_ptr = bootblock_ptr->directory_entries;     // where directory entries begin in bootblock structure 
    inode_ptr = (inode_t*)(bootblock_ptr + 1);           // where inodes begin in memory
    datablock_ptr = (datablock_t*)(1 + bootblock_ptr + bootblock_ptr->inodes_num);      // +1 offset because inodes starts from 1 and structure is zero indexed
    
    int i = 0;

    while(bytes_read < length && bytes_read < (uint32_t)(inode_curr->length)){

        index_curr_datablock = (bytes_read + offset)/FOUR_KB;      // finding index of current datablock from current inode
        if(bytes_read + offset >= inode_curr->length ||     // if offset + bytes currently read exceed size of current inode's datablock OR invalid datablock index then exit loop
            index_curr_datablock >= 1023){                  // checking if curr_datablock exceeds max number of datablocks possible
                // flag = 1;
                break;
        }

        // at this point, datablock index is valid and we look to copy data over to buffer
        datablock_curr = (datablock_t*)(datablock_ptr + inode_curr->datablock[index_curr_datablock]);   // getting to current datablock in memory structure
        data_curr = datablock_curr->data;
        for(i = (offset + bytes_read) % FOUR_KB; (bytes_read < (uint32_t)(inode_curr->length)) && (i < FOUR_KB) && (bytes_read < length); i++){        // iterating through datablock
            buf[bytes_read] = data_curr[i];     // filling buffer with the data of the current datablock (single byte at a time)
            bytes_read++;                       // moving onto the next byte to read
        }
    }
    return bytes_read;      // returning the total number of bytes read from the file onto the buf
}

// File Functions

/* file_open
* INPUTS: fname, filedescriptor
* OUTPUTS: none
* RETURN: 0 on success and -1 if file isn't valid
* SIDE EFFECTS: initializes structs and uses dentry_by_name to open by file
*/
int32_t file_open(const uint8_t *fname){
    dentry_t dentry;
    // parameter validation
    uint32_t length = strlen((int8_t*)fname);
    if(length > 32 ||         // checking if file name greater than max length 
        fname == NULL ||                                    // filename points to NULL
        read_dentry_by_name(fname, &dentry) == -1){         // check if file exists in directory
        return -1;      // return -1 on failure
    }
    return 0;            // return 0 on success
}

/* file_close
* INPUTS: filedescriptor
* OUTPUTS: none
* RETURN: 0 on success and -1 on failure
* SIDE EFFECTS: responsible for closing file
*/
int32_t file_close(int32_t filedescriptor){
    return (filedescriptor < 0 || filedescriptor > 7) ? -1 : 0;     // if filedescriptor invalid then return -1 else return 0 to indicate success
}


/* file_read
* INPUTS: filedescriptor, buf, n
* OUTPUTS: none
* RETURN: number of bytes read from file and copied over to buffer
* SIDE_EFFECTS: calling read_data file system function to read data from file into buf
*/
int32_t file_read(int32_t fd, void* buf, int32_t n){

    pcb_t* curr_pcb = terminal_arr[running_terminal].curr_pcb;
    int bytes_read = 0;
    if(n == 0){
        return 0; 
    }
    
    bytes_read = read_data((curr_pcb->fd_array[fd]).inode, (curr_pcb->fd_array[fd]).fpos, buf, n);      // int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
    
    if (bytes_read != -1){ 
        (curr_pcb->fd_array[fd]).fpos += bytes_read;
    }
    return bytes_read;
}


/* file_write
* INPUTS: filedescriptor, buf, n
* OUTPUTS: none
* RETURN: -1 always
* SIDE EFFECTS: always returns -1 since this is a read-only file system
*/
int32_t file_write(int32_t filedescriptor, const void* buf, int32_t n){
    if(filedescriptor < 0 || filedescriptor > 7 || buf == NULL){        // parameter validation: checking validity of filedescriptor and buf NULL check
        return -1;
    }
    return -1;      // can do parameter validation for filedescriptor using cases but return value is -1 always
}


// Directory Functions

/* directory_open
* INPUTS: fname, filedescriptor
* OUTPUTS: none
* RETURN: 0 on success and -1 on failure
* SIDE EFFECTS: responsible for opening a directory and return success/failure status of operation
*/
int32_t directory_open(const uint8_t *fname){
    uint32_t length = strlen((int8_t*)fname);
    if(length > 32 || fname == NULL){       // parameter validation: checking if filename exceeeds max length or if file ptr is NULL
        return -1;      // return -1 to indicate failure
    }
    read_directory_index = 0;
    return 0;           // return 0 to indicate success
}

/* directory_close
* INPUTS: filedescriptor
* OUTPUTS: none
* RETURNS: 0 on success and -1 on failure
* SIDE EFFECTS: responsible for closing file and returning status of operation
*/
int32_t directory_close(int32_t filedescriptor){                    // DOES NOTHING FOR 3.2 EXCEPT RETURN 0
    read_directory_index = 0;
    return (filedescriptor < 0 || filedescriptor > 7) ? -1 : 0;     // if filedescriptor invalid then return -1 else return 0 to indicate success
}


/* directory_read
* INPUTS: filedescriptor, buf, n
* OUTPUTS: none
* RETURN: 0 on success and -1 on failure
* SIDE EFFECTS: responsible for reading n bytes from directory to buf based on filedescriptor
*/
int32_t directory_read(int32_t filedescriptor, void* buf_arg, int32_t n){
    uint8_t* buf = (uint8_t*) buf_arg;
    pcb_t* curr_pcb = terminal_arr[running_terminal].curr_pcb;
    // parameter validation
    if (filedescriptor < 2 || filedescriptor > 7 || buf == NULL) {  // checking for invalid file descriptor, buf NULL check
        return -1;                                                  // return -1 on failure
    }

    int8_t* dentry_filename;
    int numBytes = 0;

    dentry_filename = dentry_ptr[(curr_pcb->fd_array[filedescriptor]).fpos].filename;
    if (strlen((int8_t*)dentry_filename) > 32) {
        strncpy((int8_t*)(&buf[0]), dentry_filename, 32);
        numBytes += 32;
    } else {
        strncpy((int8_t*)(&buf[0]), dentry_filename, strlen((int8_t*)dentry_filename));
        numBytes += strlen(dentry_filename);
    }
    
    (curr_pcb->fd_array[filedescriptor]).fpos++;
    if ((curr_pcb->fd_array[filedescriptor]).fpos <= 17) {
        return numBytes;
    }
    return 0;
}

/* directory_write
* INPUTS: filedescriptor, buf, n
* OUTPUTS: none
* RETURN: -1 always
* SIDE EFFECTS: always returns -1 since this is a read-only file system
*/
int32_t directory_write(int32_t filedescriptor, const void* buf, int32_t n){      // DOES NOTHING FOR 3.2 EXCEPT RETURN -1
    if(filedescriptor < 0 || filedescriptor > 7 || buf == NULL || n == 0){      // parameter validation: checking validity of filedescriptor, buffer NULL check, and bytes to be written
        return -1;
    }
    return -1;      // can do parameter validation for filedescriptor using cases but return value is -1 always
}
