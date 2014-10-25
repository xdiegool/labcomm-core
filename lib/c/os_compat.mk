## Macros
UNAME_S=$(shell uname -s)

ifeq ($(UNAME_S),Linux)
  CFLAGS=-std=c99 -g -Wall -Werror -O3  -I. -Itest -I2006
  CC=$(CROSS_COMPILE)gcc
  LD=$(CROSS_COMPILE)ld
  LDFLAGS=-L.
  LDLIBS=-llabcomm -llabcomm2006 -lrt
  MAKESHARED=gcc -o $1 -shared -Wl,-soname,$2 $3 -lc -lrt
else ifeq ($(UNAME_S),Darwin)
  CC=$(CROSS_COMPILE)clang
  LD=$(CROSS_COMPILE)ld
  CFLAGS=-g -Wall -Werror -O3  -I. -Itest \
	 -DLABCOMM_COMPAT=\"labcomm_compat_osx.h\" \
	 -Wno-tautological-compare -Wno-unused-function
  LDFLAGS=-L.
  LDLIBS=-llabcomm -llabcomm2006 
  MAKESHARED=clang -o $1 -shared -Wl,-install_name,$2 $3 -lc
else ifneq ($(findstring CYGWIN,$(UNAME_S)),)
  CFLAGS=-std=c99 -g -Wall -Werror -O3  -I. -Itest
  CC=$(CROSS_COMPILE)gcc
  LD=$(CROSS_COMPILE)ld
  LDFLAGS=-L.
  LDLIBS=-llabcomm -lrt
  ALL_DEPS:=$(filter-out %.so.1, $(ALL_DEPS)) # No -fPIC supported in windows?
else
  $(error Unknown system $(UNAME_S))
endif

ifeq ($(CROSS_COMPILE),i586-wrs-vxworks-)
  ALL_DEPS:=$(filter-out %.so.1, $(ALL_DEPS)) # PIC is only supported for RTPs
  CFLAGS:=$(CFLAGS) -DLABCOMM_COMPAT=\"labcomm_compat_vxworks.h\"
endif
