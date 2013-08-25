/*
 * Adapted from "libuv-chat" by Ben Noordhuis
 * https://github.com/bnoordhuis/libuv-chat
 */

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h> // offsetof
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "generic_queue.h"
#include "server.h"

#define SERVER_ADDR "0.0.0.0" // a.k.a. "all interfaces"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - offsetof(type, member)))


static void* users[2];


int
main(int argc, char** argv) {

    if (argc != 2){
        fprintf(stderr, "usage: %s port\n", argv[0]);
        return 1;
    }

    int s_port = atoi(argv[1]);

    QUEUE_INIT(&users);

    uv_tcp_t server_handle;
    uv_tcp_init(uv_default_loop(), &server_handle);

    const struct sockaddr_in addr = uv_ip4_addr(SERVER_ADDR, s_port);
    if (uv_tcp_bind(&server_handle, addr)){
        fatal("uv_tcp_bind");
    }

    const int backlog = 128;
    if (uv_listen((uv_stream_t*) &server_handle, backlog, on_connection)) {
        fatal("uv_listen");
    }

    printf("Listening at %s:%d\n", SERVER_ADDR, s_port);
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    return 0;
}


static void
on_connection(uv_stream_t* server_handle, int status) {
    assert(status == 0);

    // hurray, a new user!
    struct user *user = xmalloc(sizeof(*user));
    uv_tcp_init(uv_default_loop(), &user->handle);

    if (uv_accept(server_handle, (uv_stream_t*) &user->handle)){
        fatal("uv_accept");
    }

    // add him to the list of users
    QUEUE_INSERT_TAIL(&users, &user->queue);
    make_user_id(user);

    // now tell everyone
    broadcast("* %s joined from %s\n", user->id, addr_and_port(user));

    // start accepting messages from the user
    uv_read_start((uv_stream_t*) &user->handle, on_alloc, on_read);
}


static uv_buf_t
on_alloc(uv_handle_t* handle, size_t suggested_size) {
    // Return a buffer that wraps a static buffer.
    // Safe because our on_read() allocations never overlap.
    static char buf[512];
    return uv_buf_init(buf, sizeof(buf));
}


static void 
on_read(uv_stream_t* handle, ssize_t nread, uv_buf_t buf) {
    struct user *user = container_of(handle, struct user, handle);

    if (nread == -1) {
        // user disconnected
        QUEUE_REMOVE(&user->queue);
        uv_close((uv_handle_t*) &user->handle, on_close);
        broadcast("* %s has left the building\n", user->id);
        return;
    }

    // broadcast message
    broadcast("%s said: %.*s", user->id, (int) nread, buf.base);
}


static void
on_write(uv_write_t *req, int status) {
    free(req);
}


static void
on_close(uv_handle_t* handle) {
    struct user *user = container_of(handle, struct user, handle);
    free(user);
}


static void
fatal(const char *what) {
    uv_err_t err = uv_last_error(uv_default_loop());
    fprintf(stderr, "%s: %s\n", what, uv_strerror(err));
    exit(1);
}


static void
broadcast(const char *fmt, ...) {
    QUEUE* q;
    char msg[512];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    QUEUE_FOREACH(q, &users) {
        struct user *user = container_of(q, struct user, queue);
        unicast(user, msg);
    }
}


static void
unicast(struct user *user, const char *msg) {
    size_t len = strlen(msg);
    uv_write_t *req = xmalloc(sizeof(*req) + len);
    void *addr = req + 1;
    memcpy(addr, msg, len);
    uv_buf_t buf = uv_buf_init(addr, len);
    uv_write(req, (uv_stream_t*) &user->handle, &buf, 1, on_write);
}


static void
make_user_id(struct user *user) {
    // most popular baby names in Alabama in 2011
    static const char *names[] = {
        "Mason", "Ava", "James", "Madison", "Jacob", "Olivia", "John",
        "Isabella", "Noah", "Addison", "Jayden", "Chloe", "Elijah",
        "Elizabeth", "Jackson", "Abigail"
    };
    static unsigned int index0 = 0;
    static unsigned int index1 = 1;

    snprintf(user->id, sizeof(user->id), "%s %s", names[index0], names[index1]);
    index0 = (index0 + 3) % ARRAY_SIZE(names);
    index1 = (index1 + 7) % ARRAY_SIZE(names);
}


static const char
*addr_and_port(struct user *user) {
    struct sockaddr_in name;
    int namelen = sizeof(name);
    if (uv_tcp_getpeername(&user->handle, (struct sockaddr*) &name, &namelen)){
        fatal("uv_tcp_getpeername");
    }

    char addr[16];
    static char buf[32];
    uv_inet_ntop(AF_INET, &name.sin_addr, addr, sizeof(addr));
    snprintf(buf, sizeof(buf), "%s:%d", addr, ntohs(name.sin_port));

    return buf;
}


static void
*xmalloc(size_t len) {
    void *ptr = malloc(len);
    assert(ptr != NULL);
    return ptr;
}
