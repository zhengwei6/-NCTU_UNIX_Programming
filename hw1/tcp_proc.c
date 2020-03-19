#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include "tcp_proc.h"


net_tcp4 *create_tcp4_table(int *total_len, char *file_path)
{
    FILE *fp;
    net_tcp4 *tcp4_array = NULL;
    char *line = NULL;
    int lines_count = 0;
    size_t len = 0;
    int read;
    char delim[] = " ";

    // count how many lines in file
    fp = fopen(file_path, "r");
    if (fp == NULL) {
        printf("open file error\n");
        return tcp4_array; 
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        lines_count += 1;
    }
    fclose(fp);
    // malloc net_tcp4
    tcp4_array = (net_tcp4 *)malloc(sizeof(net_tcp4) * lines_count);

    // store every line info
    fp = fopen(file_path, "r");
    if (fp == NULL) {
        printf("open file error\n");
        return tcp4_array; 
    }
    int table_index = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        char *ptr = strtok(line, delim);
        int col_index = 0;
        while (ptr != NULL) {
            if (col_index == 0) {
                memset(tcp4_array[table_index].sl, '\0', sizeof(tcp4_array[table_index].sl));
                strcpy(tcp4_array[table_index].sl, ptr);
            }
            else if (col_index == 1) {
                memset(tcp4_array[table_index].local_address, '\0', sizeof(tcp4_array[table_index].local_address));
                strcpy(tcp4_array[table_index].local_address, ptr);
            }
            else if (col_index == 2) {
                memset(tcp4_array[table_index].rem_address, '\0', sizeof(tcp4_array[table_index].rem_address));
                strcpy(tcp4_array[table_index].rem_address, ptr);
            }
            else if (col_index == 7) {
                memset(tcp4_array[table_index].uid, '\0', sizeof(tcp4_array[table_index].uid));
                strcpy(tcp4_array[table_index].uid, ptr);
            }
            else if (col_index == 9) {
                memset(tcp4_array[table_index].inode, '\0', sizeof(tcp4_array[table_index].inode));
                strcpy(tcp4_array[table_index].inode, ptr);
            }
            col_index++;
            ptr = strtok(NULL, delim);
        }
        table_index++;
    }
    
    *total_len = lines_count;
    return tcp4_array;
    
}

void print_tcp4_table(const net_tcp4 *tcp_table, const int total_len) 
{
    int i;
    for (i = 0 ; i < total_len; i++) {
        printf("%s ",tcp_table[i].sl);
        printf("%s ",tcp_table[i].local_address);
        printf("%s ",tcp_table[i].rem_address);
        printf("%s ",tcp_table[i].uid);
        printf("%s\n",tcp_table[i].inode);
    }
}


void read_fd(net_tcp4 *net_table, char *dir_path, const int total_len)
{
    struct dirent *pDirent;
    DIR *pDir;
    char buffer[300];
    char pathname[300];
    char link_buf[50];
    int i;
    memset(buffer, '\0', 300);
    strcpy(buffer, dir_path);
    strcat(buffer, "/fd/");
    pDir = opendir (buffer);
    if (pDir == NULL) {
        //printf("Can't open sub dir\n");
        return;
    }
    while ((pDirent = readdir(pDir)) != NULL) {
        if (pDirent->d_name[0] == '.') {
            continue;
        }
        memset(pathname, '\0', 300);
        memset(link_buf, '\0', 50);
        strcpy(pathname,buffer);
        strcat(pathname, pDirent->d_name);
        readlink(pathname, link_buf, 50);
        if (strncmp(link_buf, "socket:", 6) == 0) {
            
            char *tmptr = &link_buf[8];
            tmptr[strlen(tmptr)-1] = '\0';
            //search table
            for (i = 0 ; i < total_len; i++) {
                if (strcmp(tmptr,net_table[i].inode) == 0) {
                    printf("%s\n",tmptr);
                }
            }
        }
        else if(strncmp(link_buf, "[0000]", 7) == 0) {

        }
    }
    return;
}

int check_num(const char *name)
{
    return atoi(name);
}

void netstat_tcp4(net_tcp4 *net_table, char *dir_path, const int total_len)
{
    struct dirent *pDirent;
    char **dir_array = NULL;
    int i;
    DIR *pDir;
    pDir = opendir(dir_path);
    if (pDir == NULL) {
        //printf("Can't open dir\n");
        return;
    }
    while ((pDirent = readdir(pDir)) != NULL) {
        char buffer[300];
        if (check_num(pDirent->d_name) == 0) {
            continue;
        }
        memset(buffer, '\0', 300);
        strcpy(buffer, dir_path);
        strcat(buffer, pDirent->d_name);
        DIR *tmpDir;
        tmpDir = opendir (buffer);
        if (tmpDir == NULL) {
            //printf("Can't open sub dir\n");
            continue;
        }
        else {
            //read fd
            read_fd(net_table,buffer, total_len);
        }
        closedir(tmpDir);
    }

}
