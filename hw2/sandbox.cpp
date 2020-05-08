#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <regex> 
#include <string.h>
#include <string>
typedef std::string string;
string wrong_option = 
    "usage: ./sandbox [-p sopath] [-d basedir] [--] cmd [cmd args ...]\n \
    -p: set the path to sandbox.so, default = ./sandbox.so\n \
    -d: the base directory that is allowed to access, default = .\n \
    --: seperate the arguments for sandbox and for the executed command\n";

int main(int argc, char *argv[])
{
    int cmd_opt;
    string so_path = "./sandbox.so";
    string dir_path = "./";
    string cmd;
    while (1) {
        cmd_opt = getopt(argc, argv, "p:d:-");
        if (cmd_opt == -1) {
            break;
        }
        switch (cmd_opt)
        {
            case 'p':
                so_path  = string(optarg);
                break;
            case 'd':
                dir_path = string(optarg);
                break;
            case '-':
                break;
            case '?':
                std::printf("%s",wrong_option.c_str());
                break;
            default:
                std::printf("%s",wrong_option.c_str());
        }
    }
    cmd = "COFDIR=" + dir_path + " LD_PRELOAD=" + so_path + " ";
    if (argc > optind) {
        cmd += string(argv[optind]);
        optind++;
        for (int i = optind ; i < argc ; i++) {
            cmd += " ";
            cmd += argv[i];
        }
        std::system(cmd.c_str());
    }
    else {
        printf("%s\n", "no command given.");
    }
    
}