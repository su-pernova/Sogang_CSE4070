#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "process.h"

typedef int pid_t;
#define PID_ERROR ((pid_t)-1)

void syscall_init (void);

/* Proj 1 implementation */
void sys_exit(int status);
int sys_read(int fd, void *buffer, unsigned length);
int sys_write(int fd, const void *buffer, unsigned length);
int sys_fibonacci(int n);
int sys_max_of_four_int(int a1, int a2, int a3, int a4);

#endif /* userprog/syscall.h */