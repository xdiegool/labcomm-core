/*
  labcomm2014_renaming_encoder.h -- a stacked encoder that renames samples

  Copyright 2015 Anders Blomdell <anders.blomdell@control.lth.se>

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

#ifndef __LABCOMM2014_RENAMING_ENCODER_H__
#define __LABCOMM2014_RENAMING_ENCODER_H__

#include "labcomm2014.h"
#include "labcomm2014_renaming.h"

struct labcomm2014_encoder *labcomm2014_renaming_encoder_new(
  struct labcomm2014_encoder *e,
  struct labcomm2014_renaming_registry *r,
  char *(*rename)(struct labcomm2014_memory *m, char *name, void *context),
  void *context);

#endif
