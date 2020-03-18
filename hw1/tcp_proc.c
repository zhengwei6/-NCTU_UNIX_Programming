#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "tcp_proc.h"

net_tcp4 *create_tcp4_table(char *file_path)
{
    FILE *fp;
    net_tcp4 *tcp4_array = NULL;
    char *line = NULL;
    int lines_count = 0;
    size_t len = 0;
    int read;

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
    
}
