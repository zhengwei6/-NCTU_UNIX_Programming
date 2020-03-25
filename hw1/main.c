#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <getopt.h>
#include <regex.h> 
#include "tcp_proc.h"
#include "tcp6_proc.h"

#define PROC_DIR "/proc/"
#define TCP4_PATH "/proc/net/tcp"
#define TCP6_PATH "/proc/net/tcp6"
#define UDP4_PATH "/proc/net/udp"
#define UDP6_PATH "/proc/net/udp6"

int mode = 0;
char filter_str[100];
regex_t regex;

int main(int argc, char *argv[]) {

    memset(filter_str, '\0', 100);
    int c;
    int option_index = 0;
    int select = 0;
    const char *optstring = "tu";
    struct option opts[] = {
        {"tcp", 0, NULL, 't'},
        {"udp", 0, NULL, 'u'},
        {0,0,0,0}
    };
    while((c = getopt_long(argc, argv, optstring, opts, &option_index)) != -1) {
        char *tmp_optarg = optarg;
        switch(c) {
            case 't':
                select = select + 1;
                break;
            case 'u':
                select = select + 2;
                break;
        }
    }
    if (argc > select) {
        if ((argc > 3) && (select == 3)) {
            strcpy(filter_str, argv[3]);
            for (int i = 4 ; i < argc; i++) {
                strcat(filter_str, " ");
                strcat(filter_str, argv[i]);
            }
        }
        if (argc >= 3 && ((select == 1) || (select == 2))) {
            strcpy(filter_str, argv[2]);
            for (int i = 3 ; i < argc ; i++) {
                strcat(filter_str, " ");
                strcat(filter_str, argv[i]);
            }
        }
        if (argc >= 2 && (select == 0)) {
            strcpy(filter_str, argv[1]);
            for (int i = 2 ; i < argc ; i++) {
                strcat(filter_str, " ");
                strcat(filter_str, argv[i]);
            }
        }
        regcomp(&regex, filter_str, 0);        
    }


    int net_table_length;
    int dir_table_length;
    int i;
    char **dir_table;
    net_tcp4 *net_table;
    net_tcp6 *net6_table;

    if ((select & 1) >= 1 || (select == 0)){
        mode = 0;
        printf("%s\n", "List of TCP connections:");
        printf("%s\n", "Proto Local Address                                "
                    "Foreign Address                              "        
                    "PID/Program name and arguments");
        // build tcp4_table
        
        net_table = create_tcp4_table(&net_table_length, TCP4_PATH);
        if (net_table == NULL) {
            return -1;
        }
        tcp4_address_form(net_table, net_table_length);
        netstat_tcp4(net_table, PROC_DIR, net_table_length);
        
        // build tcp6_table
        net6_table = create_tcp6_table(&net_table_length, TCP6_PATH);
        if (net6_table == NULL) {
            return -1;
        }
        tcp6_address_form(net6_table, net_table_length);
        netstat_tcp6(net6_table, PROC_DIR, net_table_length);
        printf("\n");
    }
    if ((select & 2) >= 1 || (select == 0)) {
        mode = 1;
        printf("%s\n", "List of UDP connections:");
        printf("%s\n", "Proto Local Address                                "
                    "Foreign Address                              "        
                    "PID/Program name and arguments");
        net_table = create_tcp4_table(&net_table_length, UDP4_PATH);
        if (net_table == NULL) {
            return -1;
        }
        tcp4_address_form(net_table, net_table_length);
        netstat_tcp4(net_table, PROC_DIR, net_table_length);
        
        net6_table = create_tcp6_table(&net_table_length, UDP6_PATH);
        if (net6_table == NULL) {
            return -1;
        }
        tcp6_address_form(net6_table, net_table_length);
        netstat_tcp6(net6_table, PROC_DIR, net_table_length);
    }
    return 0;
}
