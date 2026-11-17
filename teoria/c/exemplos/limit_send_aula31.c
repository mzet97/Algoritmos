#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/syscall.h>
#include <unistd.h>

ssize_t send(int fd, const void *buf, size_t len, int flags) {
    return syscall(SYS_sendto, fd, buf, len > 3 ? 3 : len, flags, NULL, 0);
}
