#ifndef TCP_PROC_H
#define TCP_PROC_H
#include <stdio.h>
#include <dirent.h>
#include <arpa/inet.h>
typedef struct NET_TCP_4 {
    char sl[4];
    struct sockaddr_in local_info;
    struct sockaddr_in rem_info;
    char local_address[86];
    char remote_address[86];
    char uid[10];
    char inode[10];
}net_tcp4;

net_tcp4 *create_tcp4_table(int *total_len, char *file_path);

void print_tcp4_table(net_tcp4 *tcp_table, int total_len);

void netstat_tcp4(net_tcp4 *net_table, char *dir_path, const int total_len);
#endif