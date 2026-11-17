#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

enum { MAX_CLIENTS = 32, LINE_CAP = 1024, OUT_CAP = 4096 };

typedef struct {
    int fd;
    char line[LINE_CAP];
    size_t line_len;
    char out[OUT_CAP];
    size_t out_pos;
    size_t out_len;
    int closing;
} Client;

static int nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0 ? -1 : 0;
}

static void close_client(Client *c) {
    if (c->fd >= 0) close(c->fd);
    c->fd = -1;
    c->line_len = c->out_pos = c->out_len = 0;
    c->closing = 0;
}

static int queue(Client *c, const char *data, size_t len) {
    if (c->out_pos == c->out_len) c->out_pos = c->out_len = 0;
    if (len > OUT_CAP - (c->out_len - c->out_pos)) return -1;
    if (len > OUT_CAP - c->out_len) {
        memmove(c->out, c->out + c->out_pos, c->out_len - c->out_pos);
        c->out_len -= c->out_pos;
        c->out_pos = 0;
    }
    memcpy(c->out + c->out_len, data, len);
    c->out_len += len;
    return 0;
}

static int command(Client *c) {
    size_t n = c->line_len;
    if (n == 5 && memcmp(c->line, "PING\n", 5) == 0)
        return queue(c, "PONG\n", 5);
    if (n == 5 && memcmp(c->line, "QUIT\n", 5) == 0) {
        if (queue(c, "BYE\n", 4) < 0) return -1;
        c->closing = 1;
        return 0;
    }
    if (n >= 6 && memcmp(c->line, "ECHO ", 5) == 0)
        return queue(c, c->line + 5, n - 5);
    return queue(c, "ERR unknown command\n", sizeof("ERR unknown command\n") - 1);
}

static int read_client(Client *c) {
    char bytes[1024];
    ssize_t n = recv(c->fd, bytes, sizeof(bytes), 0);
    if (n == 0) {
        c->closing = 1;
        return 0;
    }
    if (n < 0) return errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK ? 0 : -1;
    for (ssize_t i = 0; i < n; ++i) {
        if (bytes[i] == '\0' || c->line_len == LINE_CAP - 1) return -1;
        c->line[c->line_len++] = bytes[i];
        if (bytes[i] == '\n') {
            if (command(c) < 0) return -1;
            c->line_len = 0;
            if (c->closing) break;
        }
    }
    return 0;
}

static int write_client(Client *c) {
    if (c->out_pos == c->out_len) return 0;
    ssize_t n = send(c->fd, c->out + c->out_pos, c->out_len - c->out_pos,
                     MSG_NOSIGNAL);
    if (n < 0) return errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK ? 0 : -1;
    if (n == 0) return -1;
    c->out_pos += (size_t)n;
    if (c->out_pos == c->out_len) c->out_pos = c->out_len = 0;
    return 0;
}

static int listen_loopback(uint16_t port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    int opt = 1;
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(port);
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 ||
        nonblocking(fd) < 0 ||
        bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0 ||
        listen(fd, 16) < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

int main(int argc, char **argv) {
    long port = 9090;
    if (argc > 2) return 2;
    if (argc == 2) {
        char *end;
        port = strtol(argv[1], &end, 10);
        if (*end != '\0' || port < 1 || port > 65535) return 2;
    }
    int listener = listen_loopback((uint16_t)port);
    if (listener < 0) { perror("listen_loopback"); return 1; }
    Client clients[MAX_CLIENTS];
    for (size_t i = 0; i < MAX_CLIENTS; ++i) clients[i].fd = -1;
    printf("127.0.0.1:%ld\n", port);
    fflush(stdout);
    for (;;) {
        struct pollfd fds[MAX_CLIENTS + 1] = {0};
        fds[0].fd = listener;
        fds[0].events = POLLIN;
        for (size_t i = 0; i < MAX_CLIENTS; ++i) {
            Client *c = &clients[i];
            fds[i + 1].fd = c->fd;
            if (c->fd >= 0) {
                if (!c->closing) fds[i + 1].events |= POLLIN;
                if (c->out_pos < c->out_len) fds[i + 1].events |= POLLOUT;
            }
        }
        if (poll(fds, MAX_CLIENTS + 1, -1) < 0) {
            if (errno == EINTR) continue;
            perror("poll");
            break;
        }
        if (fds[0].revents & POLLIN) {
            for (size_t accepted = 0; accepted < MAX_CLIENTS; ++accepted) {
                int fd = accept(listener, NULL, NULL);
                if (fd < 0) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
                        perror("accept");
                    if (errno == EINTR) continue;
                    break;
                }
                if (nonblocking(fd) < 0) { close(fd); continue; }
                size_t slot = 0;
                while (slot < MAX_CLIENTS && clients[slot].fd >= 0) ++slot;
                if (slot == MAX_CLIENTS) { close(fd); continue; }
                clients[slot] = (Client){.fd = fd};
            }
        }
        for (size_t i = 0; i < MAX_CLIENTS; ++i) {
            Client *c = &clients[i];
            if (c->fd < 0) continue;
            short events = fds[i + 1].revents;
            if (events & (POLLERR | POLLNVAL)) { close_client(c); continue; }
            if ((events & (POLLIN | POLLHUP)) && !c->closing && read_client(c) < 0) {
                close_client(c);
                continue;
            }
            if ((events & POLLOUT) && write_client(c) < 0) {
                close_client(c);
                continue;
            }
            if (c->closing && c->out_pos == c->out_len) close_client(c);
        }
    }
    for (size_t i = 0; i < MAX_CLIENTS; ++i) close_client(&clients[i]);
    close(listener);
    return 1;
}
