/*
  labcomm2006_memory.c -- dynamic memory handlig dispatcher

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

#include "labcomm2006_private.h"

void *labcomm2006_memory_alloc(struct labcomm2006_memory *m, int lifetime, 
			   size_t size) 
{
  return m->alloc(m, lifetime, size);
}

void *labcomm2006_memory_realloc(struct labcomm2006_memory *m, int lifetime, 
			     void *ptr, size_t size) 
{
  return m->realloc(m, lifetime, ptr, size);
}

void labcomm2006_memory_free(struct labcomm2006_memory *m, int lifetime, 
			 void *ptr)
{
  m->free(m, lifetime, ptr);
}
