#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H
//1、将interrupt frame指针读出来
//2、通过esp求出压在用户程序调用栈中的系统调用号和参数，都是int
//3、通过调用好转向特定的系统调用函数，并将参数传入此调用函数
//4、待系统调用函数返回之后，将返回值写到eax寄存器中
#include <debug.h>
#include <list.h>
#include <stdint.h>

/* My Implementation */
#include "threads/alarm.h"
#include "threads/synch.h"
/* == My Implementation */

/* States in a thread's life cycle. */
enum thread_status
  {
    THREAD_RUNNING,     /* Running thread. */
    THREAD_READY,       /* Not running but ready to run. */
    THREAD_BLOCKED,     /* Waiting for an event to trigger. */
    THREAD_DYING        /* About to be destroyed. */
  };

/* Thread identifier type.
   You can redefine this to whatever type you like. */
typedef int tid_t;
#define TID_ERROR ((tid_t) -1)          /* Error value for tid_t. */

/* Thread priorities. */
#define PRI_MIN 0                       /* Lowest priority. */
#define PRI_DEFAULT 31                  /* Default priority. */
#define PRI_MAX 63                      /* Highest priority. */

/* My Implementation */
#define NICE_MAX 20
#define NICE_DEFAULT 0
#define NICE_MIN -20

#ifdef USERPROG
# define RET_STATUS_DEFAULT 0xcdcdcdcd
#endif
/* == My Implementation */

/* A kernel thread or user process.

   Each thread structure is stored in its own 4 kB page.  The
   thread structure itself sits at the very bottom of the page
   (at offset 0).  The rest of the page is reserved for the
   thread's kernel stack, which grows downward from the top of
   the page (at offset 4 kB).  Here's an illustration:

        4 kB +---------------------------------+
             |          kernel stack           |
             |                |                |
             |                |                |
             |                V                |
             |         grows downward          |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             +---------------------------------+
             |              magic              |
             |                :                |
             |                :                |
             |               name              |
             |              status             |
        0 kB +---------------------------------+

   The upshot of this is twofold:

      1. First, `struct thread' must not be allowed to grow too
         big.  If it does, then there will not be enough room for
         the kernel stack.  Our base `struct thread' is only a
         few bytes in size.  It probably should stay well under 1
         kB.

      2. Second, kernel stacks must not be allowed to grow too
         large.  If a stack overflows, it will corrupt the thread
         state.  Thus, kernel functions should not allocate large
         structures or arrays as non-static local variables.  Use
         dynamic allocation with malloc() or palloc_get_page()
         instead.

   The first symptom of either of these problems will probably be
   an assertion failure in thread_current(), which checks that
   the `magic' member of the running thread's `struct thread' is
   set to THREAD_MAGIC.  Stack overflow will normally change this
   value, triggering the assertion. */
/* The `elem' member has a dual purpose.  It can be an element in
   the run queue (thread.c), or it can be an element in a
   semaphore wait list (synch.c).  It can be used these two ways
   only because they are mutually exclusive: only a thread in the
   ready state is on the run queue, whereas only a thread in the
   blocked state is on a semaphore wait list. */
struct thread
  {
    /* Owned by thread.c. */
    tid_t tid;                          /* Thread identifier. */
    enum thread_status status;          /* Thread state. */
    char name[16];                      /* Name (for debugging purposes). */
    uint8_t *stack;                     /* Saved stack pointer. */
    int priority;                       /* Priority. */
    struct list_elem allelem;           /* List element for all threads list. */

    /* Shared between thread.c and synch.c. */
    struct list_elem elem;              /* List element. */
    
    /* My Implementation */
    struct alarm alrm;                  /* alarm object */
    int base_priority;                  /* priority before donate, if nobody donates, then it should be same as priority */
    struct list locks;                  /* the list of locks that it holds */
    bool donated;                       /* whether the thread has been donated priority */
    struct lock *blocked;               /* by which lock this thread is blocked */
    
    int nice;                           /* nice value of a thread */
    int recent_cpu;                     /* recent cpu usage */
    /* == My Implementation */
    
#ifdef USERPROG
    /* Owned by userprog/process.c. */
    uint32_t *pagedir;                  /* Page directory. */
    
    /* My Implementation */
    struct semaphore wait;              /* semaphore for process_wait */
    int ret_status;                     /* return status */
    struct list files;                  /* all opened files */
    struct file *self;                  /* the image file on the disk */
    struct thread *parent;              /* parent process */
    /* == My Implementation */
#endif

    /* Owned by thread.c. */
    unsigned magic;                     /* Detects stack overflow. */
  };

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
extern bool thread_mlfqs;

void thread_init (void);
void thread_start (void);

void thread_tick (void);
void thread_print_stats (void);

typedef void thread_func (void *aux);
tid_t thread_create (const char *name, int priority, thread_func *, void *);

void thread_block (void);
void thread_unblock (struct thread *);

struct thread *thread_current (void);
tid_t thread_tid (void);
const char *thread_name (void);

void thread_exit (void) NO_RETURN;
void thread_yield (void);

/* Performs some operation on thread t, given auxiliary data AUX. */
typedef void thread_action_func (struct thread *t, void *aux);
void thread_foreach (thread_action_func *, void *);

/* My Implementation */
void sort_thread_list (struct list *l);
void thread_set_priority_other (struct thread *curr, int new_priority, bool forced);
void thread_yield_head (struct thread *curr);

void thread_calculate_load_avg (void);
void thread_calculate_recent_cpu (void);
void thread_calculate_priority (void);
void thread_calculate_recent_cpu_for_all (void);
void thread_calculate_priority_for_all (void);
struct thread *get_thread_by_tid (tid_t);
/* == My Implementation */

int thread_get_priority (void);
void thread_set_priority (int);

int thread_get_nice (void);
void thread_set_nice (int);
int thread_get_recent_cpu (void);
int thread_get_load_avg (void);

#endif /* threads/thread.h */
