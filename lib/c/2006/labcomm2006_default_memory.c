/*
  test_default_memory.c -- LabComm default memory allocator

  Copyright 2013 Anders Blomdell <anders.blomdell@control.lth.se>

  This file is part of LabComm.

  LabComm is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LabComm is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include "labcomm2006.h"
#include "labcomm2006_private.h"

void *default_alloc2006(struct labcomm2006_memory *m, int lifetime, size_t size)
{
  return malloc(size);
}

void *default_realloc2006(struct labcomm2006_memory *m, int lifetime, 
		      void *ptr, size_t size)
{
  return realloc(ptr, size);
}

void default_free2006(struct labcomm2006_memory *m, int lifetime, void *ptr)
{
  free(ptr);
}

struct labcomm2006_memory memory2006 = {
  .alloc = default_alloc2006,
  .realloc = default_realloc2006,
  .free = default_free2006,
  .context = NULL
};

struct labcomm2006_memory *labcomm2006_default_memory = &memory2006;
