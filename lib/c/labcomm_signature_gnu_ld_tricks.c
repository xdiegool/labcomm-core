/*
  labcomm_signature_gnu_ld_tricks.h -- signature handling.

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

#include <errno.h>
#include "labcomm.h"
#include "labcomm_signature.h"
#include "labcomm_private.h"

extern struct labcomm_signature labcomm_first_signature;
extern struct labcomm_signature labcomm_last_signature;

int labcomm_signature_local_index(struct labcomm_signature *s)
{
  int result = -ENOENT;
  
  if (&labcomm_first_signature <= s && s < &labcomm_last_signature) {
    result = (int)(s - &labcomm_first_signature) + LABCOMM_USER;
  }
  return result;
}
