#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/synch.h"

static void syscall_handler (struct intr_frame *);

void syscall_init (void){
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void syscall_handler (struct intr_frame *f){  
	void *addr = (f->esp);
	int syscall = (*(uint64_t *)addr);
	int break_flag = 0;
	pid_t temp; struct file *fp;
	
	switch(syscall){
		/* Proj 1 implementation */
		case SYS_HALT:
			shutdown_power_off();
			break;

		case SYS_EXIT:
			if(is_user_vaddr(f->esp+4) == 0) sys_exit(-1);
			sys_exit(*(uint32_t*)(f->esp+4));
			break;

		case SYS_EXEC :
			if(is_user_vaddr(f->esp+4) == 0) sys_exit(-1);
			f->eax = process_execute((char*)*(uint64_t*)(f->esp+4));
			break;

		case SYS_WAIT :
			if(is_user_vaddr(f->esp+4)==0) sys_exit(-1);
			temp = (int)*(uint64_t*)(f->esp+4);
			f->eax = process_wait(temp);
			break;

		case SYS_READ:
			if(is_user_vaddr((void*)*(uint32_t*)(f->esp+8)) == 0) sys_exit(-1);
			lock_acquire(&file_lock);
			if((int)*(uint32_t*)(f->esp+4) == 0){
				for(int i=0; i<(unsigned)*(uint32_t*)(f->esp+12); i++){
					uint8_t c = input_getc();
					if(c=='\0') {
						f->eax = i;
						break;
					}
				} lock_release(&file_lock);
				break;
			}
			else if((int)*(uint32_t*)(f->esp+4) >= 3){
				if(thread_current()->fd[(int)*(uint32_t*)(f->esp+4)] == NULL){
					lock_release(&file_lock);
					sys_exit(-1);
				}
				lock_release(&file_lock);
				f->eax = file_read(thread_current()->fd[(int)*(uint32_t*)(f->esp+4)],
						 (void*)*(uint32_t*)(f->esp+8),(unsigned)*(uint32_t*)(f->esp+12));
				break;
			}
			else{
				lock_release(&file_lock);
				f->eax =  -1;
				break;
			}

		case SYS_WRITE:
			if(is_user_vaddr((const void*)*(uint32_t*)(f->esp+8)) == 0) sys_exit(-1);
			lock_acquire(&file_lock);
			if((int)*(uint32_t*)(f->esp+4) == 1){
				putbuf((char*)(const void*)*(uint32_t*)(f->esp+8), (size_t)(unsigned)*(uint32_t*)(f->esp+12));
				lock_release(&file_lock);
				f->eax = (unsigned)*(uint32_t*)(f->esp+12);
				break;
			}
			else if((int)*(uint32_t*)(f->esp+4) >= 3){
				lock_release(&file_lock);
				f->eax = file_write(thread_current()->fd[(int)*(uint32_t*)(f->esp+4)],
						 (const void*)*(uint32_t*)(f->esp+8),(unsigned)*(uint32_t*)(f->esp+12));
				break;
			}
			else {
				lock_release(&file_lock);
				f->eax = -1;
				break;
			}
			
		case SYS_FIBO:
			f->eax=sys_fibonacci((int)*(uint64_t*)(f->esp+4));
			break;

		case SYS_MAX4:
			f->eax=sys_max_of_four_int((int)*(uint64_t*)(f->esp+4),
				   (int)*(uint64_t*)(f->esp+8),
				   (int)*(uint64_t*)(f->esp+12),
				   (int)*(uint64_t*)(f->esp+16));
			break;

		/* Proj 2 implementation */
		case SYS_CREATE:
			if(is_user_vaddr(f->esp+4) == 0) sys_exit(-1);
			if(is_user_vaddr(f->esp+8) == 0) sys_exit(-1);
			if((const char*)*(uint32_t*)(f->esp+4) == NULL) sys_exit(-1);
			f->eax = filesys_create((const char*)*(uint32_t*)(f->esp+4),(unsigned)*(uint32_t*)(f->esp+8));
			break;

		case SYS_REMOVE:
			if(is_user_vaddr(f->esp+4) == 0) sys_exit(-1);
			f->eax = filesys_remove((const char*)*(uint32_t*)(f->esp+4));
			break;

		case SYS_OPEN:
			if(is_user_vaddr(f->esp+4) == 0) sys_exit(-1);
			if((const char*)*(uint32_t*)(f->esp+4) == NULL) sys_exit(-1);
			lock_acquire(&file_lock);
			fp = filesys_open((const char*)*(uint32_t*)(f->esp+4));
			if(fp == NULL){
				lock_release(&file_lock);
				f->eax = -1;
				break;
			}
			else{
				for(int i = 3; i<128; i++){
					if(thread_current()->fd[i] == NULL){
						thread_current()->fd[i] = fp;
						lock_release(&file_lock);
						f->eax =  i;
						break_flag = 1;
						break;
					}
				} if(break_flag) break;
			} lock_release(&file_lock);
			f->eax = -1;
			break;

		case SYS_CLOSE:
			if(is_user_vaddr(f->esp+4) == 0) sys_exit(-1);
			if(thread_current()->fd[(int)*(uint32_t*)(f->esp+4)] == NULL) sys_exit(-1);
			file_close(thread_current()->fd[(int)*(uint32_t*)(f->esp+4)]);
			thread_current()->fd[(int)*(uint32_t*)(f->esp+4)] = NULL;
			break;

		case SYS_FILESIZE:
			if(is_user_vaddr(f->esp+4) == 0) sys_exit(-1);
			if(thread_current()->fd[(int)*(uint32_t*)(f->esp+4)]==NULL) sys_exit(-1);
			f->eax =  file_length(thread_current()->fd[(int)*(uint32_t*)(f->esp+4)]);
			break;

		case SYS_SEEK:
			if(is_user_vaddr(f->esp+4) == 0) sys_exit(-1);
			if(is_user_vaddr(f->esp+8) == 0) sys_exit(-1);
			if(thread_current()->fd[(int)*(uint32_t*)(f->esp+4)] == NULL) sys_exit(-1);
			file_seek(thread_current()->fd[(int)*(uint32_t*)(f->esp+4)], (unsigned)*(uint32_t*)(f->esp+8));
			break;

		case SYS_TELL:
			if(is_user_vaddr(f->esp+4) == 0) sys_exit(-1);
			if(thread_current()->fd[(int)*(uint32_t*)(f->esp+4)] == NULL) sys_exit(-1);
			f->eax = file_tell(thread_current()->fd[(int)*(uint32_t*)(f->esp+4)]);
			break;
   } 
	return;
}

/* Proj 1 implementation */
int sys_fibonacci (int n){
	int first = 1, second = 1, new;
	if(n==1 || n==2) return 1;
	else{
		for(int i=2; i<n; i++){
			new = first+second;
			first = second;
			second = new;
		} return new; }
}

int sys_max_of_four_int(int a1, int a2, int a3, int a4){
	int max = a1;
	if(max<a2) max = a2;
	if(max<a3) max = a3;
	if(max<a4) max = a4;
	return max;
}

/* Proj 1,2 implementation */
void sys_exit (int status){
	printf("%s: exit(%d)\n", thread_name(), status);
	thread_current()->exit_stat = status;

	int idx = 3;
	while(idx<128){
		if(thread_current()->fd[idx] != NULL) {
			file_close(thread_current()->fd[idx]);
			thread_current()->fd[idx] = NULL;
		}idx++;
	}

	struct list_elem *e = list_begin(&(thread_current())->child_list);
	struct thread *t = list_entry(e, struct thread, child_element);

	while(e != list_end(&(thread_current()->child_list))){
		process_wait(t->tid);
		e = list_next(e);
		t = list_entry(e, struct thread, child_element);
	}

	file_close(thread_current()->current_file);
	thread_exit();
}