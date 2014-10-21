#include "sandbox-ipc.h"
#include <unistd.h>

SandboxIPC::SandboxIPC(int _dupAs)
  : dupAs (_dupAs)
{
  int ipc_fds[2];
  socketpair (AF_UNIX, SOCK_STREAM, 0, ipc_fds);
  child = ipc_fds[IPC_CHILD_IDX];
  parent = ipc_fds[IPC_PARENT_IDX];
}

SandboxIPC::~SandboxIPC()
{
  stopPoll();
  close (child);
  close (parent);
}

bool
SandboxIPC::dup()
{
  if (dup2 (child, dupAs) != dupAs)
    return false;
  return true;
}

void
SandboxIPC::setCallback(SandboxIPCCallback cb, void* user_data)
{
  m_cb = cb;
  m_cb_data = user_data;
}

void
SandboxIPC::cb_forward(uv_poll_t* req, int status, int events)
{
  SandboxIPC* self = static_cast<SandboxIPC*>(req->data);
  self->m_cb (*self, self->m_cb_data);
}

bool
SandboxIPC::startPoll(uv_loop_t* loop)
{
  uv_poll_init_socket (loop, &poll, parent);
  poll.data = this;
  uv_poll_start (&poll, UV_READABLE, SandboxIPC::cb_forward);
  return true; //FIXME: check errors
}

bool
SandboxIPC::stopPoll()
{
  uv_poll_stop (&poll);
  return true; //FIXME: check errors
}
