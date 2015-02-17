/*
  labcomm_ioctl.h -- labcomm ioctl declarations

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

#ifndef __LABCOMM_IOCTL_H__
#define __LABCOMM_IOCTL_H__

#include "labcomm.h"

/*
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | |   |                         |               |               |  
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | |   |                         |               |
 *  | |   |                         |               +- number    (8)
 *  | |   |                         +----------------- type      (8)
 *  | |   +------------------------------------------- size      (13)
 *  | +----------------------------------------------- direction (2)
 *  +------------------------------------------------- signature (1)
 *  
 * type 0-31     are reserved for labcomm library use
 */


#define LABCOMM_IOC_NRBITS      8
#define LABCOMM_IOC_TYPEBITS    8
#define LABCOMM_IOC_SIZEBITS   13
#define LABCOMM_IOC_DIRBITS     2
#define LABCOMM_IOC_SIGBITS     1
#define LABCOMM_IOC_NRMASK     ((1 << LABCOMM_IOC_NRBITS)-1)
#define LABCOMM_IOC_TYPEMASK   ((1 << LABCOMM_IOC_TYPEBITS)-1)
#define LABCOMM_IOC_SIZEMASK   ((1 << LABCOMM_IOC_SIZEBITS)-1)
#define LABCOMM_IOC_DIRMASK    ((1 << LABCOMM_IOC_DIRBITS)-1)
#define LABCOMM_IOC_SIGMASK    ((1 << LABCOMM_IOC_SIGBITS)-1)
#define LABCOMM_IOC_NRSHIFT    0
#define LABCOMM_IOC_TYPESHIFT  (LABCOMM_IOC_NRSHIFT+LABCOMM_IOC_NRBITS)
#define LABCOMM_IOC_SIZESHIFT  (LABCOMM_IOC_TYPESHIFT+LABCOMM_IOC_TYPEBITS)
#define LABCOMM_IOC_DIRSHIFT   (LABCOMM_IOC_SIZESHIFT+LABCOMM_IOC_SIZEBITS)
#define LABCOMM_IOC_SIGSHIFT   (LABCOMM_IOC_DIRSHIFT+LABCOMM_IOC_DIRBITS)

#define LABCOMM_IOC_NOSIG       0U
#define LABCOMM_IOC_USESIG      1U

#define LABCOMM_IOC_NONE        0U
#define LABCOMM_IOC_WRITE       1U
#define LABCOMM_IOC_READ        2U

#define LABCOMM_IOC(signature,dir,type,nr,size)	  \
  (((signature) << LABCOMM_IOC_SIGSHIFT) |	  \
   ((dir)       << LABCOMM_IOC_DIRSHIFT) |	  \
   ((size)      << LABCOMM_IOC_SIZESHIFT) |	  \
   ((type)      << LABCOMM_IOC_TYPESHIFT) |	  \
   ((nr)        << LABCOMM_IOC_NRSHIFT))	  

#define LABCOMM_IOC_SIG(nr) \
  (((nr) >> LABCOMM_IOC_SIGSHIFT) & LABCOMM_IOC_SIGMASK)
#define LABCOMM_IOC_DIR(nr) \
  (((nr) >> LABCOMM_IOC_DIRSHIFT) & LABCOMM_IOC_DIRMASK)
#define LABCOMM_IOC_SIZE(nr) \
  (((nr) >> LABCOMM_IOC_SIZESHIFT) & LABCOMM_IOC_SIZEMASK)
#define LABCOMM_IOC_TYPE(nr) \
  (((nr) >> LABCOMM_IOC_TYPESHIFT) & LABCOMM_IOC_TYPEMASK)
#define LABCOMM_IOC_NR(nr) \
  (((nr) >> LABCOMM_IOC_NRSHIFT) & LABCOMM_IOC_NRMASK)

#define LABCOMM_IO(type,nr)						\
  LABCOMM_IOC(LABCOMM_IOC_NOSIG,LABCOMM_IOC_NONE,type,nr,0)
#define LABCOMM_IOR(type,nr,size)					\
  LABCOMM_IOC(LABCOMM_IOC_NOSIG,LABCOMM_IOC_READ,type,nr,sizeof(size))
#define LABCOMM_IOW(type,nr,size)					\
  LABCOMM_IOC(LABCOMM_IOC_NOSIG,LABCOMM_IOC_WRITE,type,nr,sizeof(size))
#define LABCOMM_IOS(type,nr)                                    \
  LABCOMM_IOC(LABCOMM_IOC_USESIG,LABCOMM_IOC_READ,type,nr,0)
#define LABCOMM_IOSR(type,nr,size)					\
  LABCOMM_IOC(LABCOMM_IOC_USESIG,LABCOMM_IOC_READ,type,nr,sizeof(size))
#define LABCOMM_IOSW(type,nr,size)					\
  LABCOMM_IOC(LABCOMM_IOC_USESIG,LABCOMM_IOC_WRITE,type,nr,sizeof(size))

#define LABCOMM_IOCTL_WRITER_GET_BYTES_WRITTEN \
  LABCOMM_IOR(0,1,int)
#define LABCOMM_IOCTL_WRITER_GET_BYTE_POINTER \
  LABCOMM_IOR(0,2,void*)

#endif
