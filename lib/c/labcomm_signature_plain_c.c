/*
  labcomm_signature_plain_c.c -- signature handling.

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

#define LABCOMM_DECLARE_SIGNATURE(name) \
  struct labcomm_signature __attribute__((aligned(1))) name

#include <errno.h>
#include "labcomm.h"
#include "labcomm_signature.h"
#include "labcomm_private.h"

/* NB: We need to mimic forward declarations done in generated code */
LABCOMM_DECLARE_SIGNATURE(labcomm_sentinel_1);
#define LABCOMM_GEN_CODE
#include ALL_LABCOMM_FILES
#undef LABCOMM_GEN_CODE
LABCOMM_DECLARE_SIGNATURE(labcomm_sentinel_2);

/* NB: We need to force our sentinels out of bss segment, hence -1 */
LABCOMM_DECLARE_SIGNATURE(labcomm_sentinel_1) = { -1, NULL, NULL, 0, NULL};
#define LABCOMM_GEN_SIGNATURE
#include ALL_LABCOMM_FILES
#undef LABCOMM_GEN_SIGNATURE
LABCOMM_DECLARE_SIGNATURE(labcomm_sentinel_2) = { -1, NULL, NULL, 0, NULL};

int labcomm_signature_local_index(struct labcomm_signature *s)
{
  int result = -ENOENT;
  struct labcomm_signature *labcomm_first_signature;
  struct labcomm_signature *labcomm_last_signature;

  if (&labcomm_sentinel_1 < &labcomm_sentinel_2) {
    labcomm_first_signature = &labcomm_sentinel_1;
    labcomm_last_signature = &labcomm_sentinel_2;
  } else {
    labcomm_first_signature = &labcomm_sentinel_2;
    labcomm_last_signature = &labcomm_sentinel_1;
  }
  if (labcomm_first_signature <= s && s < labcomm_last_signature) {
    result = (int)(s - labcomm_first_signature) + LABCOMM_USER - 1;
  }
  return result;
}
