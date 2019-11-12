#include<xinu.h>

uint32 active_spinlocks = 0; 

syscall sl_initlock(sl_lock_t *l){
	if(active_spinlocks == NSPINLOCKS){
		return SYSERR; 	
	}
	else{
		l->value = 0; 
		l->owner = 0; 
		active_spinlocks++;
	}
	return OK; 
}


syscall sl_lock(sl_lock_t *l){
	while(test_and_set(&l->value, 1)){}
	l->owner = currpid; 
	return OK; 
}

syscall sl_unlock(sl_lock_t *l){
	if(l->owner != currpid){
		return SYSERR;
	} 
	test_and_set(&l->value, 0);
	l->owner = 0; 
	return OK; 

}
