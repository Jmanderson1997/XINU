/* lock.h - structs for locks */

typedef struct sl_lock_t {
	uint32 value; 
	pid32 owner;  
}sl_lock_t; 

typedef struct lock_t {
	uint32 flag; 
	uint32 guard; 
	qid16 queue;   
	pid32 owner;  
}lock_t; 

typedef struct pi_lock_t {
	uint32 flag; 
	uint32 guard; 
	qid16 queue;   
	pid32 owner;  
	uint32 index; 
	pri16 highest_prio;
	pid32 prio_owner;
}pi_lock_t; 

extern pi_lock_t* pi_locks[]; 

#define NSPINLOCKS 20
#define NLOCKS 20
#define NPILOCKS 20
#define NOLOCK 21
