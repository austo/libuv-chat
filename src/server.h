#ifndef SERVER_H
#define SERVER_H

#include <uv.h>


struct user {
  void* queue[2]; // linked list
  char id[32];
  uv_tcp_t handle;
  uv_work_t work;
  uv_buf_t buf;
};


static void *
xmalloc(size_t len);

static void
fatal(const char *what);

static void
unicast(struct user *user, const char *msg);

static void
broadcast(const char *fmt, ...);

static void
make_user_id(struct user *user);

static const char *
addr_and_port(struct user *user);

static uv_buf_t
on_alloc(uv_handle_t* handle, size_t suggested_size);

static void
on_read(uv_stream_t* handle, ssize_t nread, uv_buf_t buf);

static void
on_write(uv_write_t *req, int status);

static void
on_close(uv_handle_t* handle);

static void
on_connection(uv_stream_t* server_handle, int status);

static void
new_user_work(uv_work_t *req);

static void 
new_user_after(uv_work_t *req);

static void
broadcast_work(uv_work_t *req);

static void 
broadcast_after(uv_work_t *req);


#endif