#ifndef TCP6_PROC_H
#define TCP6_PROC_H
#include <stdio.h>
#include <dirent.h>
#include <arpa/inet.h>

typedef struct NET_TCP_6 {
    char sl[4];
    struct sockaddr_in6 local_info;
    struct sockaddr_in6 rem_info;
    char local_address[47];
    char remote_address[47];
    char uid[10];
    char inode[10];
}net_tcp6;

net_tcp6 *create_tcp6_table(int *total_len, char *file_path);

void tcp6_address_form(net_tcp6 *tcp_table, int total_len);

void netstat_tcp6(net_tcp6 *net_table, char *dir_path, const int total_len);
#endif