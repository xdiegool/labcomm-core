/*
  labcomm2014_error.h -- labcomm2014 error declarations

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

#ifndef __LABCOMM2014_ERROR_H___
#define __LABCOMM2014_ERROR_H___

enum labcomm2014_error {
#define LABCOMM2014_ERROR(name, description) name ,
#include "labcomm2014_error.h"
#undef LABCOMM2014_ERROR
};

struct labcomm2014_error_handler;

void labcomm2014_error_warning(struct labcomm2014_error_handler *e,
			   enum labcomm2014_error,
			   char *format,
			   ...);
			 
void labcomm2014_error_fatal_global(enum labcomm2014_error error,
				char *format,
				...);
			 
#endif

#ifdef LABCOMM2014_ERROR

LABCOMM2014_ERROR(LABCOMM2014_ERROR_SIGNATURE_ALREADY_SET,
                  "Signature has already been set")
LABCOMM2014_ERROR(LABCOMM2014_ERROR_SIGNATURE_NOT_SET,
                  "Signature has not been set")

LABCOMM2014_ERROR(LABCOMM2014_ERROR_ENC_NO_REG_SIGNATURE,
                  "Encoder has no registration for this signature")
LABCOMM2014_ERROR(LABCOMM2014_ERROR_ENC_BUF_FULL,
                  "The labcomm2014 buffer is full")
LABCOMM2014_ERROR(LABCOMM2014_ERROR_DEC_UNKNOWN_DATATYPE,
                  "Decoder: Unknown datatype")
LABCOMM2014_ERROR(LABCOMM2014_ERROR_DEC_INDEX_MISMATCH,
                  "Decoder: index mismatch")
LABCOMM2014_ERROR(LABCOMM2014_ERROR_DEC_TYPE_NOT_FOUND,
                  "Decoder: type not found")
LABCOMM2014_ERROR(LABCOMM2014_ERROR_UNIMPLEMENTED_FUNC,
                  "This function is not yet implemented")
LABCOMM2014_ERROR(LABCOMM2014_ERROR_MEMORY,
                  "Could not allocate memory")
LABCOMM2014_ERROR(LABCOMM2014_ERROR_USER_DEF,
                  "User defined error")
LABCOMM2014_ERROR(LABCOMM2014_ERROR_BAD_WRITER,
                  "Decoder: writer_ioctl() failed")

#endif
