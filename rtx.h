/* @brief: rtx.h User API prototype
 * @author: G013
 * @last edition date: 2014/02/05
 */
#ifndef RTX_H_
#define RTX_H_

/* ----- Definitations ----- */
#define RTX_ERR -1
#define NULL 0
#define NUM_USER_PROCS 11
/* Process Priority. The bigger the number is, the lower the priority is*/
#define SYSTEM  0
#define HIGH    1
#define MEDIUM  2
#define LOW     3
#define LOWEST  4

/* ----- Types ----- */
typedef unsigned int U32;

/* initialization table item */
typedef struct proc_init
{	
	int m_pid;	        /* process id */ 
	int m_priority;         /* initial priority, not used in this example. */ 
	int m_stack_size;       /* size of stack in words */
	void (*mpf_start_pc) ();/* entry point of the process */    
} PROC_INIT;

typedef struct _user_msgbuf {
	int mtype; /* user defined message type */
	char mtext[1]; /* body of the message */
} MSGBUF;

/* ----- RTX User API ----- */
#define __SVC_0  __svc_indirect(0)

extern void k_rtx_init(void);
#define rtx_init() _rtx_init((U32)k_rtx_init)
extern void __SVC_0 _rtx_init(U32 p_func);

extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
extern int __SVC_0 _release_processor(U32 p_func);

extern void *k_request_memory_block(void);
#define request_memory_block() _request_memory_block((U32)k_request_memory_block)
extern void *_request_memory_block(U32 p_func) __SVC_0;
/* __SVC_0 can also be put at the end of the function declaration */

extern int k_release_memory_block(void *);
#define release_memory_block(p_mem_blk) _release_memory_block((U32)k_release_memory_block, p_mem_blk)
extern int _release_memory_block(U32 p_func, void *p_mem_blk) __SVC_0;

extern int k_set_process_priority(int, int);
#define set_process_priority(process_id, priority) _set_process_priority((U32)k_set_process_priority, process_id, priority)
extern int _set_process_priority(U32 p_func, int process_id, int priority) __SVC_0;

extern int k_get_process_priority(int);
#define get_process_priority(process_id) _get_process_priority((U32)k_get_process_priority, process_id)
extern int _get_process_priority(U32 p_func, int process_id) __SVC_0;

extern void* k_receive_message(int*);
#define receive_message(sender_pid) _receive_message((U32)k_receive_message, sender_pid)
extern void* _receive_message(U32 p_func, int* sender_pid) __SVC_0;

extern int k_send_message(int, void*);
#define send_message(process_id, env) _send_message((U32) k_send_message, process_id, env)
extern int _send_message(U32 p_func, int process_id, void* env) __SVC_0;

extern int k_delayed_send(int, void *, int);
#define delayed_send(process_id, env, delay) _delayed_send((U32) k_delayed_send, process_id, env, delay);
extern int _delayed_send(U32 p_func, int process_id, void* env, int delay) __SVC_0;
#endif /* !RTX_H_ */
