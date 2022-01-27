#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);

void syscall_init (void){
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/* Proj 1 implementation */
static void syscall_handler (struct intr_frame *f){
	void *addr = (f->esp);
	int syscall = (*(uint64_t *)addr);
	pid_t temp;
	switch(syscall){
		case SYS_HALT:
			shutdown_power_off();
			break;

		case SYS_EXIT:
			if(is_user_vaddr(f->esp+4)==0) sys_exit(-1);
			sys_exit((int)*(uint64_t*)(f->esp+4));
			break;

		case SYS_EXEC :
			if(is_user_vaddr(f->esp+4)==0) sys_exit(-1);
			f->eax = process_execute((char*)*(uint64_t*)(f->esp+4));
			break;

		case SYS_WAIT :
			if(is_user_vaddr(f->esp+4)==0) sys_exit(-1);
			temp = (int)*(uint64_t*)(f->esp+4);
			f->eax = process_wait(temp);
			break;

		case SYS_READ:
			f->eax = sys_read ((int)*(uint64_t*)(f->esp+4),
				 	 (void*)*(uint64_t*)(f->esp+8),
					 (unsigned)*(uint64_t*)(f->esp+12));
			break;

		case SYS_WRITE:
			f->eax = sys_write((int)*(uint64_t*)(f->esp+4),
					 (const void*)*(uint64_t*)(f->esp+8),
					 (unsigned)*(uint64_t*)(f->esp+12));
			break;

		case SYS_FIBO:
			f->eax=sys_fibonacci((int)*(uint64_t*)(f->esp+4));
			break;

		case SYS_MAX4:
			f->eax=sys_max_of_four_int((int)*(uint64_t*)(f->esp+4),
				   (int)*(uint64_t*)(f->esp+8),
				   (int)*(uint64_t*)(f->esp+12),
				   (int)*(uint64_t*)(f->esp+16));
			break;
   } 
	return;
}

void sys_exit (int status){
	printf("%s: exit(%d)\n", thread_name(), status);
	thread_current()->exit_stat = status;
	thread_exit();
}

int sys_read (int fd, void *buffer, unsigned length){
	int i;
	if(fd == 0){
		for(i=0; i<(int)length; i++){
			uint8_t c = input_getc();
			if(c=='\0') break;
		} return i;
	}
	else return -1;
}

int sys_write (int fd, const void *buffer, unsigned length){
	if(fd == 1){
		putbuf((char*)buffer, (size_t)length);
		return length;
	}
	else return -1;
}

int sys_fibonacci (int n){
	int first=1, second=1, new;
	if(n==1 || n==2) return 1;
	else{
		for(int i=2; i<n; i++){
			new = first+second;
			first = second;
			second = new;
		} return new;
	}
}

int sys_max_of_four_int(int a1, int a2, int a3, int a4){
	int max = a1;
	if(max<a2) max = a2;
	if(max<a3) max = a3;
	if(max<a4) max = a4;
	return max;
}