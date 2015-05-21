/*
  labcomm2014_error.c -- labcomm2014 error handling

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
#include <stdio.h>
#include <stdarg.h>
#include "labcomm2014_error.h"

void labcomm2014_error_fatal_global(enum labcomm2014_error error,
                                    char *format,
                                    ...)
{
  va_list args;

  fprintf(stderr, "Fatal error %d\n", error);
  va_start(args, format);
  vprintf(format, args);
  va_end(args);

  exit(1);
}

void labcomm2014_error_warning(struct labcomm2014_error_handler *e,
                               enum labcomm2014_error error,
                               char *format,
                               ...)
{
  va_list args;

  fprintf(stderr, "Fatal warning %d\n", error);
  va_start(args, format);
  vprintf(format, args);
  va_end(args);

  exit(1);
}
                         
