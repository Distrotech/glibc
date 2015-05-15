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

typedef enum
{
  TUNABLE_TYPE_SECURE,
  TUNABLE_TYPE_SIZE_T = 1 << 1,
  TUNABLE_TYPE_STRING = 1 << 2
} tunable_type_t;

typedef struct _tunable tunable_t;

/* Add your tunable IDs here.  */
typedef enum
{
  TUNABLES_MAX
} tunable_id_t;

#define FULL_NAME(top,ns,id) (top ## _ ## ns ## _ ## id)
#define FULL_NAME_S(top,ns,id) (#top "_" #ns "_" #id)

#define TUNABLES_NAMESPACE_BEGIN(size) \
  tunables_namespace_begin (TUNABLE_NAMESPACE, size)

/* Register a tunable.  This macro validates that the call is OK and then calls
   tunable_init to do the real work of adding the tunable and setting its value
   based on its environment variable(s).  */
#define TUNABLE_REGISTER(id,alias,val,size,type,secure) \
({									      \
  static_assert (FULL_NAME (TOP_NAMESPACE, TUNABLE_NAMESPACE, id)	      \
		 < TUNABLES_MAX);					      \
  tunable_init (FULL_NAME (TOP_NAMESPACE, TUNABLE_NAMESPACE, id),	      \
		FULL_NAME_S (TOP_NAMESPACE, TUNABLE_NAMESPACE, id),	      \
		(alias), (val), (size), (type), (secure));		      \
									      \
})

#define TUNABLES_NAMESPACE_INIT() \
  tunables_init (TUNABLE_NAMESPACE, size)
