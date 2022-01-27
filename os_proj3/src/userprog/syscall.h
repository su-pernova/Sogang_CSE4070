#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "process.h"

typedef int pid_t;
#define PID_ERROR ((pid_t)-1)
void syscall_init (void);

/* Proj 1 implementation */
int sys_fibonacci(int n);
int sys_max_of_four_int(int a, int b, int c, int d);

/* Proj 1,2 implementation */
void sys_exit(int status);

#endif /* userprog/syscall.h */