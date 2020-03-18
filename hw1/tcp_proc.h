#ifndef TCP_PROC_H
#define TCP_PROC_H
#include <stdio.h>
#include <dirent.h>

typedef struct NET_TCP_4 {
    char sl[4];
    char local_address[15];
    char rem_address[15];
    char uid[10];
    char inode[10];
}net_tcp4;

net_tcp4 *create_tcp4_table(char *file_path);

#endif