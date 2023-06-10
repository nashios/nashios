#pragma once

#include <kernel/api/posix/poll.h>
#include <sys/cdefs.h>
#include <sys/types.h>

___BEGIN_DECLS

int poll(struct pollfd fds[], nfds_t nfds, int timeout);

___END_DECLS
