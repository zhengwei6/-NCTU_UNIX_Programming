#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdio>
#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <regex> 
#include <string.h>
#include <string>

void creat_at(char *dir_path,char *relative_path)
{
	int dir_fd;
    int pid;
    std::string tmp;
	int fd;
	int flags;
	mode_t mode;
    char buf[200];
    char buf_tt[200];
	dir_fd = open(dir_path, O_RDONLY);
	if (dir_fd < 0) 
	{
		perror("open");
		exit(EXIT_FAILURE);
	}
    pid = getpid();
    tmp = "/proc/" + std::to_string(pid) + "/fd/" + std::to_string(dir_fd);
    strcpy(buf_tt, tmp.c_str()); 
    int len;
    if ((len = readlink(buf_tt, buf, sizeof(buf)-1)) != -1)
        buf[len] = '\0';
	flags = O_CREAT | O_TRUNC | O_RDWR;
	mode = 0640;
	fd = openat(dir_fd, relative_path, flags, mode);
	if (fd < 0) 
	{
		perror("openat");
		exit(EXIT_FAILURE);
	}
 
	write(fd, "HELLO", 5);
	close(fd);
	close(dir_fd);
}
 
int main()
{
    
	creat_at("./open", "./log.txt");
	return 0;
}