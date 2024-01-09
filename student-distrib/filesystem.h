#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H


#include "multiboot.h"
#include "lib.h"
#include "types.h"
#include "terminal.h"
#include "system_calls.h"

typedef struct __attribute__((packed)) dentry_struct         // struct for dentry
{
    int8_t filename[32];
    uint32_t filetype;
    uint32_t inode;
    uint32_t reserved[6];
} dentry_t;

typedef struct __attribute__((packed)) bootblock_struct{        // struct for boot block
    uint32_t directory_num;
    uint32_t inodes_num;
    uint32_t datablocks_num;
    uint32_t reserved[13];
    dentry_t directory_entries[63];
} bootblock_t;

typedef struct __attribute__((packed)) inode_struct{        // struct for inode
    uint32_t length;
    uint32_t datablock[1023];
} inode_t;

typedef struct __attribute__((packed))  datablock_struct{        // struct for data
    uint8_t data[4096];
} datablock_t;


uint32_t fstart;
bootblock_t* bootblock_ptr;
dentry_t* dentry_ptr;
inode_t* inode_ptr;
datablock_t* datablock_ptr;

// file system functions
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

// initialize bootblocker
int32_t bootblock_init(uint32_t bootblock_address);

// file functions
int32_t file_open(const uint8_t *fname);
int32_t file_close(int32_t filedescriptor);
int32_t file_read(int32_t fd, void* buf, int32_t n);
int32_t file_write(int32_t filedescriptor, const void* buf, int32_t n);

// directory functions
int32_t directory_open(const uint8_t *fname);
int32_t directory_close(int32_t filedescriptor);
int32_t directory_read(int32_t filedescriptor, void* buf_arg, int32_t n);
int32_t directory_write(int32_t filedescriptor, const void* buf, int32_t n);

#endif
