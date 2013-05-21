#ifndef __LABCOMM_ERROR_H__
#define __LABCOMM_ERROR_H__

enum labcomm_error {
#define LABCOMM_ERROR(name, description) name ,
#include "labcomm_error.h"
#undef LABCOMM_ERROR
};

#endif

#ifdef LABCOMM_ERROR

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
