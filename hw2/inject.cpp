#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <cstring>
#include <dlfcn.h>
#include <linux/limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <string>
#include <fcntl.h>
#include <stdarg.h>

static char conf_dir[PATH_MAX];
static char buffer[PATH_MAX];
#define path_not_allowed(func_name, path) \
        sprintf(buffer, "[sandbox] %s: access to %s is not allowed\n", func_name, path);\
        write_to_terminal(buffer);

#define exc_not_allowed(func_name, path) \
        sprintf(buffer, "[sandbox] %s(%s): not allowed\n", func_name, path);\
        write_to_terminal(buffer);

#define absolute_path(path) char cwd[PATH_MAX];\
                      char resolved_path[PATH_MAX];\
                      realpath(path, resolved_path);\
                      realpath(conf_dir, cwd)

#define macro_readlink(path_in, buf_in, bufsiz_in) \
        ((* (ssize_t (*)(const char *path, char *buf, size_t bufsiz)) dlsym(RTLD_NEXT, "readlink"))(path_in, buf_in, bufsiz_in))

void init() {
    char *tmp = getenv("COFDIR");
    int i = 0;
    while (tmp) {
        *(conf_dir + i) = *(tmp++);
        if (*(conf_dir + i) == '\0') {
            break;
        }
        i++;
    }
    return;
}

void write_to_terminal(char *buf) {
    int terminal_fd;
    terminal_fd = (* (int (*)(const char *pathname, int flags, ...)) dlsym(RTLD_NEXT, "open"))("/dev/tty", O_RDWR);
    if (terminal_fd == -1) {
        printf("error open terminal\n");
    }
    (* (ssize_t (*)(int fd, const void *buf, size_t count)) dlsym(RTLD_NEXT, "write"))(terminal_fd, buf, strlen(buf));
    return;
}

int check_path(const char *path1, const char *path2) {
    return strncmp(path1, path2, strlen(path2));
}

int system(const char *command) {
    errno = EACCES;
    exc_not_allowed("system", command);
    return -1;
}
int execvp(const char *file, char *const argv[]) {
    errno = EACCES;
    exc_not_allowed("execv", file);
    return -1;
}

int execv(const char *path, char *const argv[]) {
    errno = EACCES;
    exc_not_allowed("execv", path);
    return -1;
}
int execlp(const char *file, const char *arg, ...) {
    errno = EACCES;
    exc_not_allowed("execlp", file);
    return -1;
}

int execle(const char *path, const char *arg, ...) {
    errno = EACCES;
    exc_not_allowed("execle", path);
    return -1;
}

int execl(const char *path, const char *arg, ...) {
    errno = EACCES;
    exc_not_allowed("execl", path);
    return -1;
}

int execve(const char *pathname, char *const argv[], char *const envp[]) {
    errno = EACCES;
    exc_not_allowed("execve", pathname);
    return -1;
}

int unlink(const char *pathname) {
    init();
    absolute_path(pathname);
    if (check_path(resolved_path, cwd)) {
        path_not_allowed("unlink", pathname);
        errno = EACCES;
        return -1;
    }
    int ret = (* (int (*)(const char *pathname)) dlsym(RTLD_NEXT, "unlink"))(pathname);
    return ret;
}

int symlink(const char *target, const char *linkpath) {
    init();
    int flag = 0;
    char cwd[PATH_MAX];
    char resolved_path1[PATH_MAX];
    char resolved_path2[PATH_MAX];
    realpath(target, resolved_path1);
    realpath(linkpath, resolved_path2);
    realpath(conf_dir, cwd);
    if (check_path(resolved_path1, cwd)) {
        flag = 1;
        path_not_allowed("symlink", target);
    }
    if (check_path(resolved_path2, cwd)) {
        flag = 1;
        path_not_allowed("symlink", linkpath);
    }
    if (flag == 1) {
        errno = EACCES;
        return -1;
    }
    int ret = (* (int (*)(const char *target, const char *linkpath)) dlsym(RTLD_NEXT, "symlink"))(target, linkpath);
    return ret;
}

int __xstat(int ver, const char * path, struct stat * stat_buf) {
    init();
    absolute_path(path);
    if (check_path(resolved_path, cwd)) {
        path_not_allowed("__xstat", path);
        errno = EACCES;
        return -1;
    }
    int ret = (* (int (*)(int ver, const char * path, struct stat * stat_buf)) dlsym(RTLD_NEXT, "__xstat"))(ver, path, stat_buf);
    return ret;
}

int __lxstat (int ver, const char * path, struct stat *stat_buf) {
    init();
    absolute_path(path);
    if (check_path(resolved_path, cwd)) {
        path_not_allowed("__lxstat", path);
        errno = EACCES;
        return -1;
    }
    int ret = (* (int (*)(int ver, const char * path, struct stat * stat_buf)) dlsym(RTLD_NEXT, "__lxstat"))(ver, path, stat_buf);
    return ret;
}

int stat(const char *path, struct stat *buf) {
    init();
    absolute_path(path);
    if (check_path(resolved_path, cwd)) {
        path_not_allowed("stat", path);
        errno = EACCES;
        return -1;
    }
    int ret = (* (int (*)(const char *path, struct stat *buf)) dlsym(RTLD_NEXT, "stat"))(path, buf);
    return ret;
}

int rmdir(const char *pathname) {
    init();
    absolute_path(pathname);
    if (check_path(resolved_path, cwd)) {
        path_not_allowed("rmdir", pathname);
        errno = EACCES;
        return -1;
    }
    int ret = (* (int (*)(const char *pathname)) dlsym(RTLD_NEXT, "rmdir"))(pathname);
    return ret;
}

int rename(const char *oldpath, const char *newpath) {
    init();
    int flag = 0;
    char cwd[PATH_MAX];
    char resolved_path1[PATH_MAX];
    char resolved_path2[PATH_MAX];
    realpath(oldpath, resolved_path1);
    realpath(newpath, resolved_path2);
    realpath(conf_dir, cwd);
    if (check_path(resolved_path1, cwd)) {
        flag = 1;
        path_not_allowed("rename", oldpath);
    }
    if (check_path(resolved_path2, cwd)) {
        flag = 1;
        path_not_allowed("rename", newpath);
    }
    if (flag == 1) {
        errno = EACCES;
        return -1;
    }
    int ret = (* (int (*)(const char *oldpath, const char *newpath)) dlsym(RTLD_NEXT, "rename"))(oldpath, newpath);
    return ret;
}

int remove(const char *pathname) {
    init();
    absolute_path(pathname);
    if (check_path(resolved_path, cwd)) {
        path_not_allowed("remove", pathname);
        errno = EACCES;
        return -1;
    }
    int ret = (* (int (*)(const char *pathname)) dlsym(RTLD_NEXT, "remove"))(pathname);
    return ret;
}

ssize_t readlink(const char *path, char *buf, size_t bufsiz) {
    init();
    absolute_path(path);
    if (check_path(resolved_path, cwd)) {
        path_not_allowed("readlink", path);
        errno = EACCES;
        return -1;
    }
    ssize_t ret = (* (ssize_t (*)(const char *path, char *buf, size_t bufsiz)) dlsym(RTLD_NEXT, "readlink"))(path, buf, bufsiz);
    return ret;
}


DIR *opendir(const char *name) {
    init();
    absolute_path(name);
    if (check_path(resolved_path, cwd)) {
        path_not_allowed("opendir", name);
        errno = EACCES;
        return NULL;
    }
    DIR * ret = (* (DIR * (*)(const char * filename)) dlsym(RTLD_NEXT, "opendir"))(name);
    return ret;
}

int openat(int dirfd, const char *pathname, int flags, ...) {
    init();
    mode_t mode;
    va_list al;
	va_start(al, flags);
	mode = va_arg(al, mode_t);
	va_end(al);
    int ret;

    char path[PATH_MAX];
    if (dirfd != AT_FDCWD) {
        std::string tmp;
        int len;
        int pid = getpid();
        tmp = "/proc/" + std::to_string(pid) + "/fd/" + std::to_string(dirfd);
        if ((len = macro_readlink(tmp.c_str(), path, sizeof(path)-1)) != -1)
            path[len] = '\0';
        if (path[strlen(path) - 1] != '/') {
            path[len ] = '/';
            path[len + 1] = '\0';
        }
        strcat(path, pathname);
    }
    else {
        strcpy(path, pathname);
    }
    absolute_path(path);
    if (check_path(resolved_path, cwd)) {
        path_not_allowed("openat",path);
        errno = EACCES;
        return -1;
    }
    if(mode > 0777) {
        ret = (* (int (*)(int dirfd, const char *pathname, int flags, ...)) dlsym(RTLD_NEXT, "openat"))(dirfd, pathname, flags);
    }
    else {
        ret = (* (int (*)(int dirfd, const char *pathname, int flags, ...)) dlsym(RTLD_NEXT, "openat"))(dirfd, pathname, flags, mode);
    }
    return ret;
}

int open(const char *pathname, int flags, ...) {
    init();
    mode_t mode;
    va_list al;
	va_start(al, flags);
	mode = va_arg(al, mode_t);
	va_end(al);
    absolute_path(pathname);
    int ret;
    if (check_path(resolved_path, cwd)) {
        path_not_allowed("open",pathname);
        errno = EACCES;
        return -1;
    }
    if(mode > 0777) {
        ret = (* (int (*)(const char *pathname, int flags, ...)) dlsym(RTLD_NEXT, "open"))(pathname, flags);
    }
    else {
        ret = (* (int (*)(const char *pathname, int flags, ...)) dlsym(RTLD_NEXT, "open"))(pathname, flags, mode);
    }
    return ret;
}

int mkdir(const char *pathname, mode_t mode) {
    init();
    absolute_path(pathname);
    if (check_path(resolved_path, cwd)) {
        path_not_allowed("mkdir",pathname);
        errno = EACCES;
        return -1;
    }
    int ret =(* (int (*)(const char * pathname, mode_t mode)) dlsym(RTLD_NEXT, "mkdir"))(pathname, mode);
    return ret;
}

int link(const char *oldpath, const char *newpath) {
    init();
    int flag = 0;
    char cwd[PATH_MAX];
    char resolved_path1[PATH_MAX];
    char resolved_path2[PATH_MAX];
    realpath(oldpath, resolved_path1);
    realpath(newpath, resolved_path2);
    realpath(conf_dir, cwd);
    if (check_path(resolved_path1, cwd)) {
        flag = 1;
        path_not_allowed("link", oldpath);
    }
    if (check_path(resolved_path2, cwd)) {
        flag = 1;
        path_not_allowed("link", newpath);
    }
    if (flag == 1) {
        errno = EACCES;
        return -1;
    }
    int ret = (* (int (*)(const char *oldpath, const char *newpath)) dlsym(RTLD_NEXT, "link"))(oldpath, newpath);
    return ret;
}

FILE *fopen(const char *pathname, const char *mode) {
    init();
    absolute_path(pathname);
    if (check_path(resolved_path, cwd)) {
        path_not_allowed("fopen",pathname);
        errno = EACCES;
        return NULL;
    }
    FILE * ret = (* (FILE * (*)(const char *pathname, const char *mode)) dlsym(RTLD_NEXT, "fopen"))(pathname, mode);
    return ret;
}

int creat(const char *path, mode_t mode) {
    init();
    absolute_path(path);
    if (check_path(resolved_path, cwd)) {
        path_not_allowed("create",path);
        errno = EACCES;
        return -1;
    }
    int ret = (* (int (*)(const char *path, mode_t mode)) dlsym(RTLD_NEXT, "create"))(path, mode);
    return ret;
}

int chown(const char *pathname, uid_t owner, gid_t group) {
    init();
    absolute_path(pathname);
    if (check_path(resolved_path, cwd)) {
        path_not_allowed("chown",pathname);
        errno = EACCES;
        return -1;
    }
    int ret = (* (int (*)(const char * pathname, uid_t owner, gid_t group)) dlsym(RTLD_NEXT, "chown"))(pathname, owner, group);
    return ret;
}

int chmod(const char *pathname, mode_t mode) {
    init();
    absolute_path(pathname);
    if (check_path(resolved_path, cwd)) {
        path_not_allowed("chmod",pathname);
        errno = EACCES;
        return -1;
    }
    int ret =(* (int (*)(const char * pathname, mode_t mode)) dlsym(RTLD_NEXT, "chmod"))(pathname, mode);
    return ret;
}

int chdir(const char *path) {
    init();
    absolute_path(path);
    if (check_path(resolved_path, cwd)) {
        path_not_allowed("chdir", path);
        errno = EACCES;
        return -1;
    }
    int ret =(* (int (*)(const char * path)) dlsym(RTLD_NEXT, "chdir"))(path);
    return ret;
}




