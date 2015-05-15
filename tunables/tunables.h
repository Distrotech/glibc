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

#include "tunable-list.h"

typedef void (*tunable_setter_t) (const char *);
typedef struct _tunable tunable_t;

extern void tunables_namespace_begin (tunable_id_t, size_t);
extern void tunable_register (tunable_id_t, const char *, const char *,
			      tunable_setter_t, bool);
extern void tunables_init (tunable_id_t);

/* Build a full tunable name from a top namespace, tunable namespace and the
   id.  */
#define FULL_NAME(top,ns,id) FULL_NAME1 (top,ns,id)
#define FULL_NAME1(top,ns,id) (top ## _ ## ns ## _ ## id)

#define FULL_NAME_S(top,ns,id) (#top "_" #ns "_" #id)

#define NS_NAME(top, ns) NS_NAME1(top, ns)
#define NS_NAME1(top, ns) (top ## _ ## ns)

/* Start registering tunables in the current namespace.  */
#define TUNABLES_NAMESPACE_BEGIN(size) \
  tunables_namespace_begin (NS_NAME(TOP_NAMESPACE, TUNABLE_NAMESPACE), size)

/* Register a tunable.  This macro validates that the call is OK and then calls
   tunable_init to do the real work of adding the tunable and setting its value
   based on its environment variable(s).  */
#define TUNABLE_REGISTER(id,alias,set) \
({									      \
  tunable_register (FULL_NAME (TOP_NAMESPACE, TUNABLE_NAMESPACE, id),	      \
		    FULL_NAME_S (TOP_NAMESPACE, TUNABLE_NAMESPACE, id),	      \
		    (alias), (set), false);				      \
									      \
})

/* Does exactly the same thing as TUNABLE_REGISTER, except that it allows the
   tunable to look for environment variable values even for setuid binaries.
   This is a separate macro and not just another parameter in TUNABLE_REGISTER
   to avoid accidentally setting a secure flag where it is not required.  */
#define TUNABLE_REGISTER_SECURE(id,alias,set) \
({									      \
  tunable_register (FULL_NAME (TOP_NAMESPACE, TUNABLE_NAMESPACE, id),	      \
		    FULL_NAME_S (TOP_NAMESPACE, TUNABLE_NAMESPACE, id),	      \
		    (alias), (set), true);				      \
									      \
})

/* Initialize tunables in the namespace.  */
#define TUNABLES_NAMESPACE_INIT() \
  tunables_init (NS_NAME (TOP_NAMESPACE, TUNABLE_NAMESPACE))
