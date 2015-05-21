/* Call to terminate the current thread.  NaCl version.
   Copyright (C) 2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <exit-thread.h>
#include <lowlevellock.h>
#include <nacl-interfaces.h>
#include <pthread-functions.h>
#include <stdbool.h>
#include <sysdep.h>
#include <tls.h>
#include <atomic.h>


/* A sysdeps/CPU/nacl/exit-thread.c file defines this function
   and then #include's this file.  */
static void call_on_stack (void *sp, void *func, void *arg)
  __attribute__ ((noreturn));


static void __attribute__ ((noreturn))
do_thread_exit (volatile int32_t *stack_flag)
{
  __nacl_irt_thread.thread_exit ((int32_t *) stack_flag);

  /* That never returns unless something is severely and unrecoverably
     wrong.  If it ever does, try to make sure we crash.  */
  while (1)
    __builtin_trap ();
}


static bool
multiple_threads (void)
{
#ifdef SHARED
  unsigned int *ptr_nthreads = __libc_pthread_functions.ptr_nthreads;
  PTR_DEMANGLE (ptr_nthreads);
#else
  extern unsigned int __nptl_nthreads __attribute ((weak));
  unsigned int *const ptr_nthreads = &__nptl_nthreads;
  if (ptr_nthreads == NULL)
    return false;
#endif
  return *ptr_nthreads > 1;
}


union exit_stack
{
  char stack[256] __attribute__ ((aligned (16)));
  struct
  {
    volatile int32_t flag;
    bool initialized;
  };
};
typedef union exit_stack exit_stack_t;

static exit_stack_t initial_exit_stack;
static int exit_stack_lock = LLL_LOCK_INITIALIZER;

static exit_stack_t *
get_exit_stack (void)
{
  exit_stack_t *result;

  lll_lock (exit_stack_lock, LLL_PRIVATE);

  if (!initial_exit_stack.initialized)
    {
      initial_exit_stack.initialized = true;
      atomic_store_relaxed (&initial_exit_stack.flag, 1);
    }

  while (atomic_load_relaxed (&initial_exit_stack.flag) != 0)
    {
      lll_unlock (exit_stack_lock, LLL_PRIVATE);
      __nacl_irt_basic.sched_yield ();
      lll_lock (exit_stack_lock, LLL_PRIVATE);
    }

  result = &initial_exit_stack;

  atomic_store_relaxed (&result->flag, 1);

  lll_unlock (exit_stack_lock, LLL_PRIVATE);

  return result;
}


static void __attribute__ ((noreturn))
exit_on_exit_stack (exit_stack_t *stack)
{
  struct pthread *pd = THREAD_SELF;

  /* Mark the thread as having exited and wake anybody waiting for it.
     After this, both PD itself and its real stack will be reclaimed
     and it's not safe to touch or examine them.  */
  pd->tid = 0;
  lll_futex_wake (&pd->tid, 1, LLL_PRIVATE);

  /* Now we can exit for real, and get off this exit stack.  The system
     will clear the flag some time after the exit stack is guaranteed not
     to be in use.  */
  do_thread_exit (&stack->flag);
}

static void __attribute__ ((noreturn))
exit_off_stack (void)
{
  exit_stack_t *stack = get_exit_stack ();

  call_on_stack (stack + 1, &exit_on_exit_stack, stack);
}

void
__exit_thread (bool detached)
{
  if (detached || !multiple_threads ())
    /* There is no other thread that cares when we exit, so life is simple.  */
    do_thread_exit (NULL);
  else
    /* We must exit in a way that wakes up pthread_join,
       i.e. clears and futex-wakes THREAD_SELF->tid.  */
    exit_off_stack ();
}
