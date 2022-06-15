#pragma once

#include <kernel/api/posix/sys/wait.h>
#include <kernel/api/posix/signal.h>
#include <sys/types.h>

pid_t wait(int *stat_loc);
pid_t waitpid(pid_t pid, int *stat_loc, int options);
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);
