#pragma once

#include <kernel/api/posix/poll.h>
#include <sys/types.h>

int poll(struct pollfd fds[], nfds_t nfds, int timeout);
