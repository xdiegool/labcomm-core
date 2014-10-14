/*
  labcomm.c -- runtime for handling encoding and decoding of
               labcomm samples.

  Copyright 2006-2013 Anders Blomdell <anders.blomdell@control.lth.se>

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

#ifdef LABCOMM_COMPAT
  #include LABCOMM_COMPAT
#else
  #include <stdio.h>
  #include <strings.h>
#endif

#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>

#include "labcomm.h"
#include "labcomm_private.h"
#include "labcomm_ioctl.h"
#include "labcomm_dynamic_buffer_writer.h"

#define LABCOMM_VERSION "LabComm20141009"

/* Unwrapping reader/writer functions */
#define UNWRAP_ac(rw, ac, ...) ac
#define UNWRAP(func, ...)	     \
  while (1) {								\
    if (UNWRAP_ac(__VA_ARGS__)->action->func) {				\
      return UNWRAP_ac(__VA_ARGS__)->action->func(__VA_ARGS__); }	\
    if (UNWRAP_ac(__VA_ARGS__)->next == NULL) { return -ENOSYS; }	\
    UNWRAP_ac( __VA_ARGS__) = UNWRAP_ac(__VA_ARGS__)->next;		\
  }

int labcomm_reader_alloc(struct labcomm_reader *r, 
                         struct labcomm_reader_action_context *action_context, 
                         char *labcomm_version)
{
  UNWRAP(alloc, r, action_context, labcomm_version);
}

int labcomm_reader_free(struct labcomm_reader *r, 
                        struct labcomm_reader_action_context *action_context)
{
  UNWRAP(free, r, action_context);
}

int labcomm_reader_start(struct labcomm_reader *r, 
                         struct labcomm_reader_action_context *action_context,
			 int local_index, int remote_index,
			 struct labcomm_signature *signature,
			 void *value)
{
  UNWRAP(start, r, action_context, local_index, remote_index, signature, value);
}

int labcomm_reader_end(struct labcomm_reader *r, 
                       struct labcomm_reader_action_context *action_context)
{
  UNWRAP(end, r, action_context);
}

int labcomm_reader_fill(struct labcomm_reader *r, 
                        struct labcomm_reader_action_context *action_context)
{
  UNWRAP(fill, r, action_context);
}

int labcomm_reader_ioctl(struct labcomm_reader *r, 
                         struct labcomm_reader_action_context *action_context,
                         int local_index, int remote_index,
                         struct labcomm_signature *signature, 
                         uint32_t ioctl_action, va_list args)
{
  UNWRAP(ioctl, r, action_context, 
	 local_index, remote_index, signature, ioctl_action, args);
}

int labcomm_writer_alloc(struct labcomm_writer *w, 
                         struct labcomm_writer_action_context *action_context, 
                         char *labcomm_version)
{
  UNWRAP(alloc, w, action_context, labcomm_version);
}

int labcomm_writer_free(struct labcomm_writer *w, 
                        struct labcomm_writer_action_context *action_context)
{
  UNWRAP(free, w, action_context);
}

int labcomm_writer_start(struct labcomm_writer *w, 
                         struct labcomm_writer_action_context *action_context,
                         int index, struct labcomm_signature *signature,
                         void *value)
{
  UNWRAP(start, w, action_context, index, signature, value);
}

int labcomm_writer_end(struct labcomm_writer *w, 
                       struct labcomm_writer_action_context *action_context)
{
  UNWRAP(end, w, action_context);
} 

int labcomm_writer_flush(struct labcomm_writer *w, 
                         struct labcomm_writer_action_context *action_context)
{
  UNWRAP(flush, w, action_context);
} 

int labcomm_writer_ioctl(struct labcomm_writer *w, 
                         struct labcomm_writer_action_context *action_context, 
                         int index, 
                         struct labcomm_signature *signature, 
                         uint32_t ioctl_action, va_list args)
{
  UNWRAP(ioctl, w, action_context, index, signature, ioctl_action, args);
} 

#undef UNWRAP
#undef UNWRAP_ac




static const char *labcomm_error_string[] = { 
#define LABCOMM_ERROR(name, description) description ,
#include "labcomm_error.h"
#undef LABCOMM_ERROR
};
static const int labcomm_error_string_count = (sizeof(labcomm_error_string) / 
					       sizeof(labcomm_error_string[0]));


const char *labcomm_error_get_str(enum labcomm_error error_id)
{
  const char *error_str = NULL;
  // Check if this is a known error ID.
  if (error_id < labcomm_error_string_count) {
    error_str = labcomm_error_string[error_id];
  }
  return error_str;
}

void on_error_fprintf(enum labcomm_error error_id, size_t nbr_va_args, ...)
{
#ifndef LABCOMM_NO_STDIO
  const char *err_msg = labcomm_error_get_str(error_id); // The final string to print.
  if (err_msg == NULL) {
    err_msg = "Error with an unknown error ID occured.";
  }
  fprintf(stderr, "%s\n", err_msg);

 if (nbr_va_args > 0) {
   va_list arg_pointer;
   va_start(arg_pointer, nbr_va_args);

   fprintf(stderr, "%s\n", "Extra info {");
   char *print_format = va_arg(arg_pointer, char *);
   vfprintf(stderr, print_format, arg_pointer);
   fprintf(stderr, "}\n");

   va_end(arg_pointer);
 } 
#else
 ; // If labcomm can't be compiled with stdio the user will have to make an own error callback functionif he/she needs error reporting.
#endif
}



#if 0
static void dump(void *p, int size, int first, int last)
{
  int i, j;

  printf("%d %d (%p): ", first, last, p);
  for (i = first ; i < last ; i++) {
    for (j = 0 ; j < size ; j++) {
      printf("%2.2d", ((char*)p)[(i-first)*size + j]);
    }
    printf(" ");
  }
  printf("\n");
}
#endif

void *labcomm_signature_array_ref(struct labcomm_memory *memory,
				  int *first, int *last, void **data,
				  int size, int index)
{
  if (*first == 0 && *last == 0) {
    *first = index;
    *last = index + 1;
    *data = labcomm_memory_alloc(memory, 0, size);
    if (*data) { 
      memset(*data, 0, size); 
    }
  } else if (index < *first || *last <= index) {
    void *old_data = *data;
    int old_first = *first;
    int old_last = *last;
    int n;
    *first = (index<old_first)?index:old_first;
    *last = (old_last<=index)?index+1:old_last;
    n = (*last - *first);
    *data = labcomm_memory_alloc(memory, 0, n * size);
    if (*data) {
      memset(*data, 0, n * size);
      memcpy(*data + (old_first - *first) * size, 
	     old_data, 
	     (old_last - old_first) * size);
    }
//    dump(old_data, size, old_first, old_last);
    labcomm_memory_free(memory, 0, old_data);
  }
  if (*data) {
//    dump(*data, size, *first, *last);
    return *data + (index - *first) * size;
  } else {
    return NULL;
  }
}

static int local_index = 0x40;

void labcomm_set_local_index(struct labcomm_signature *signature)
{
  if (signature->index != 0) {
    labcomm_error_fatal_global(LABCOMM_ERROR_SIGNATURE_ALREADY_SET,
			       "Signature already set: %s\n", signature->name);
  }
  signature->index = local_index;
  local_index++;
}

int labcomm_get_local_index(struct labcomm_signature *signature)
{
  if (signature->index == 0) {
    labcomm_error_fatal_global(LABCOMM_ERROR_SIGNATURE_NOT_SET,
			       "Signature not set: %s\n", signature->name);
  }
  return signature->index;
}

int labcomm_internal_sizeof(struct labcomm_signature *signature,
                            void *v)
{
  int length = signature->encoded_size(v);
  fprintf(stderr, "SIZEOF(%s) = %d %d %d\n",
          signature->name, 
          labcomm_size_packed32(signature->index),
          labcomm_size_packed32(length),
          length);
  return (labcomm_size_packed32(signature->index) +
          labcomm_size_packed32(length) +
          length);
}

