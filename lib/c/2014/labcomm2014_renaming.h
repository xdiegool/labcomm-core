/*
  labcomm2014_renaming.h -- functions intended for renaming
                            encoders and decoders

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

#ifndef __LABCOMM2014_RENAMING_H__
#define __LABCOMM2014_RENAMING_H__

#include "labcomm2014.h"

char *labcomm2014_renaming_prefix(struct labcomm2014_memory *m,
                                  char *name, void *context);

char *labcomm2014_renaming_suffix(struct labcomm2014_memory *m,
                                  char *name, void *context);

#endif
