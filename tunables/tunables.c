/* The tunable framework.  See the README to know how to use the tunable in
   a glibc module.

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

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/param.h>
#include "tunables.h"

extern char **__environ;

/* A tunable.  */
struct _tunable
{
  char *name;
  char *alias;
  void *val;
  size_t size;
  tunable_type_t type;
  bool initialized;
};

/* The full list of tunables.  */
static tunable_t tunable_list[TUNABLES_MAX];


void
tunables_namespace_begin (tunable_id_t id, size_t size)
{
  tunable_list[id].size = size;
}

/* Set the value of a tunable from its environment variable(s).  */
static void
tunable_set (tunable_t *t, const char *val)
{
  switch (t->type & ~TUNABLE_TYPE_SECURE)
    {
    case TUNABLE_TYPE_SIZE_T:
      *(size_t *) t->val = strtoul (val, NULL, 0);
    case TUNABLE_TYPE_STRING:
      memcpy (t->val, val, t->size);
    default:
      __builtin_trap ();
    }
  t->initialized = true;
}

/* Initialize all tunables in the namespace group specified by ID.  */
void
tunables_init (tunable_id_t id)
{
  int end = tunable_list[id].size;

  /* Traverse through the environment to find environment variables we may need
     to set.  */
  char **envp = __environ;
  while (*envp != NULL)
    {
      char *envline = *envp;
      int len = 0;

      while (envline[len] != '\0' && envline[len] != '=')
	len++;

      /* Just the name and no value.  */
      if (envline[len] == '\0')
	continue;

      for (int i = id; i < end; i++)
	{
	  /* Skip over tunables that are either initialized or are not safe to
	     load for setuid binaries.  */
	  if ((__libc_enable_secure
	       && (tunable_list[id].type & TUNABLE_TYPE_SECURE) == 0)
	      || tunable_list[id].initialized)
	    continue;

	  const char *name = tunable_list[id].name;
	  const char *alias = tunable_list[id].alias;
	  char *val = NULL;

	  if (memcmp (envline, name, MIN(len, strlen (name))) == 0)
	    val = &envline[len + 1];
	  else if (memcmp (envline, alias, MIN(len, strlen (alias))) == 0)
	    val = &envline[len + 1];

	  if (val != NULL)
	    {
	      tunable_set (&tunable_list[id], val);
	      break;
	    }
	}
      envp++;
    }
}

/* Initialize a tunable and set its value via the set environment variable.  */
void
tunable_register (tunable_id_t id, const char *name, const char *alias,
		void *val, size_t size, tunable_type_t type)
{
  tunable_list[id].name = __strdup (name);
  tunable_list[id].alias = alias ? __strdup (alias) : NULL;
  tunable_list[id].val = val;
  tunable_list[id].size = size;
  tunable_list[id].type = type;
}
