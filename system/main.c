#include <xinu.h>

void run_for_ms(uint32 time){
	uint32 start = proctab[currpid].runtime;
	while (proctab[currpid].runtime-start < time);
}

void aquire_one_lock(pi_lock_t *l){
	pi_lock(l); 
	run_for_ms(5000);
	pi_unlock(l);
}

void aquire_two_locks(pi_lock_t *l1, pi_lock_t *l2){

	pi_lock(l1);
	sleepms(150); 
	pi_lock(l2); 
	run_for_ms(5000);
	pi_unlock(l2); 
	pi_unlock(l1);
}

process main(void){

	pi_lock_t lock1, lock2; 
	pi_initlock(&lock1); 
	pi_initlock(&lock2); 

	pid32 pid1, pid2, pid3; 
	pid1 = create((void *)aquire_two_locks, INITSTK, 1,"thread1", 2, &lock1, &lock2);
	pid2 = create((void *)aquire_one_lock, INITSTK, 1,"thread2", 1, &lock2);
	pid3 = create((void *)aquire_one_lock, INITSTK, 3,"thread3", 1, &lock1);

	resume(pid1);
	sleepms(100);
	sync_printf("Just thread 1\n");
	sync_printf("thread %d priority: %d\n", pid1, proctab[pid1].prprio);
	resume(pid2);		
	sleepms(600);
	sync_printf("Thread 1 and 2\n");
	sync_printf("thread %d priority: %d\n",pid1, proctab[pid1].prprio); 
	sync_printf("thread %d priority: %d\n", pid2, proctab[pid2].prprio);
	resume(pid3); 
	sleepms(1000);
	sync_printf("All three threads\n");
	sync_printf("thread %d priority: %d\n", pid1, proctab[pid1].prprio); 
	sync_printf("thread %d priority: %d\n", pid2, proctab[pid2].prprio);
	sync_printf("thread %d priority: %d\n", pid3, proctab[pid3].prprio); 
	receive();
	receive(); 
	receive();
	sync_printf("After Completion\n");
	sync_printf("thread %d priority: %d\n", pid1, proctab[pid1].prprio); 
	sync_printf("thread %d priority: %d\n", pid2, proctab[pid2].prprio);
	sync_printf("thread %d priority: %d\n", pid3, proctab[pid3].prprio); 
	return OK;
}
