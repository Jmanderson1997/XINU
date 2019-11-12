/* lock.h - structs for locks */

typedef struct sl_lock_t {
	uint16 value; 
	pid32 owner;  
}sl_lock_t; 

typedef struct lock_t {
	uint16 flag; 
	uint16 guard; 
	qid16 queue;   
	pid32 owner;  
}lock_t; 

typedef struct pi_lock_t {
	uint16 flag; 
	uint16 guard; 
	qid16 queue;   
	pid32 owner;  
}pi_lock_t; 

extern pi_lock_t pi_locks[];
extern uint16 active_locks; 

#define NSPINLOCKS 20
#define NLOCKS 20
#define NPILOCKS 20
