/*
  labcomm2006_error.h -- labcomm error declarations

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

#ifndef __LABCOMM2006_ERROR_H__
#define __LABCOMM2006_ERROR_H__

enum labcomm2006_error {
#define LABCOMM_ERROR(name, description) name ,
#include "labcomm2006_error.h"
#undef LABCOMM_ERROR
};

struct labcomm2006_error_handler;

void labcomm2006_error_warning(struct labcomm2006_error_handler *e,
			   enum labcomm2006_error,
			   char *format,
			   ...);
			 
void labcomm2006_error_fatal_global(enum labcomm2006_error error,
				char *format,
				...);
			 
#endif

#ifdef LABCOMM_ERROR

LABCOMM_ERROR(LABCOMM_ERROR_SIGNATURE_ALREADY_SET, 
	      "Signature has already been set")
LABCOMM_ERROR(LABCOMM_ERROR_SIGNATURE_NOT_SET, 
	      "Signature has not been set")

LABCOMM_ERROR(LABCOMM_ERROR_ENC_NO_REG_SIGNATURE, 
	      "Encoder has no registration for this signature")
LABCOMM_ERROR(LABCOMM_ERROR_ENC_BUF_FULL,
	      "The labcomm buffer is full")
LABCOMM_ERROR(LABCOMM_ERROR_DEC_UNKNOWN_DATATYPE,
	      "Decoder: Unknown datatype")
LABCOMM_ERROR(LABCOMM_ERROR_DEC_INDEX_MISMATCH, 
	      "Decoder: index mismatch")
LABCOMM_ERROR(LABCOMM_ERROR_DEC_TYPE_NOT_FOUND,
	      "Decoder: type not found")
LABCOMM_ERROR(LABCOMM_ERROR_UNIMPLEMENTED_FUNC,
	      "This function is not yet implemented")
LABCOMM_ERROR(LABCOMM_ERROR_MEMORY,
	      "Could not allocate memory")
LABCOMM_ERROR(LABCOMM_ERROR_USER_DEF,     
	      "User defined error")

#endif
