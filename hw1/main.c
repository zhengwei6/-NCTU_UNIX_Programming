#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "dir_proc.h"
#include "tcp_proc.h"

#define PROC_DIR "/proc/"
#define TCP4_PATH "/proc/net/tcp"

int main(int argc, char *argv[]) {
    int net_table_length;
    int dir_table_length;
    int i;
    char **dir_table;
    printf("%s\n","Proto Local Address           Foreign Address         PID/Program name and arguments");
    // build tcp_table
    net_tcp4 *net_table = create_tcp4_table(&net_table_length, TCP4_PATH);
    if (net_table == NULL) {
        return -1;
    }
    print_tcp4_table(net_table, net_table_length);
    // read /proc/[pid]/fd/
    netstat_tcp4(net_table, PROC_DIR, net_table_length);
    return 0;
}
char *a = "Local Address           ";