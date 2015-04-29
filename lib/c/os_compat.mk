## Macros
UNAME_S=$(shell uname -s)

ifeq ($(UNAME_S),Linux)
  CC=$(CROSS_COMPILE)gcc
  LD=$(CROSS_COMPILE)gcc
  CFLAGS=-std=c99 -g -Wall -Werror -O3  -I. 
  CFLAGS_TEST=$(CFLAGS) -Itest
  LDFLAGS=-L..
  LDLIBS=-llabcomm$(LIBVERSION) -lrt
  MAKESHARED=gcc -o $1 -shared -Wl,-soname,$2 $3 -lc -lrt
else ifeq ($(UNAME_S),Darwin)
  #CC=$(CROSS_COMPILE)clang
  #LD=$(CROSS_COMPILE)ld
  CC=$(CROSS_COMPILE)gcc
  LD=$(CROSS_COMPILE)gcc
  CFLAGS=-g -Wall -Werror -O3  -I. -Itest \
	 -DLABCOMM_COMPAT=\"labcomm$(LIBVERSION)_compat_osx.h\" \
	 -DLABCOMM_OS_DARWIN=1
#	 -Wno-tautological-compare -Wno-unused-function
  CFLAGS+=-std=c99 
  LDFLAGS=-L.. 
  LDLIBS=-llabcomm$(LIBVERSION)
  MAKESHARED=clang -o $1 -shared -Wl,-install_name,$2 $3 -lc 
else ifneq ($(findstring CYGWIN,$(UNAME_S)),)
  CC=$(CROSS_COMPILE)gcc
  LD=$(CROSS_COMPILE)ld
  CFLAGS=-std=c99 -g -Wall -Werror -O3  -I. 
  LDFLAGS=-L..
  LDLIBS=-llabcomm$(LIBVERSION) -lrt
  ALL_DEPS:=$(filter-out %.so.1, $(ALL_DEPS)) # No -fPIC supported in windows?
else
  $(error Unknown system $(UNAME_S))
endif
CFLAGS_TEST=$(CFLAGS) -Itest -DVERSION=$(VERSION)

ifeq ($(CROSS_COMPILE),i586-wrs-vxworks-)
  ALL_DEPS:=$(filter-out %.so.1, $(ALL_DEPS)) # PIC is only supported for RTPs
  CFLAGS:=$(CFLAGS) -DLABCOMM_COMPAT=\"labcomm_compat_vxworks.h\"
endif
