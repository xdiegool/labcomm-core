## Macros
UNAME_S=$(shell uname -s)
ALL_DEPS=liblabcomm.a liblabcomm.so.1

ifeq ($(UNAME_S),Linux)
  CFLAGS=-std=c99 -g -Wall -Werror -O3  -I. -Itest
  CC=$(CROSS_COMPILE)gcc
  LD=$(CROSS_COMPILE)ld
  LDFLAGS=-L.
  LDLIBS=-llabcomm -lrt
  MAKESHARED=gcc -o $1 -shared -Wl,-soname,$2 $3 -lc -lrt
else ifeq ($(UNAME_S),Darwin)
  CC=$(CROSS_COMPILE)clang
  LD=$(CROSS_COMPILE)ld
  CFLAGS=-g -Wall -Werror -O3  -I. -Itest \
	 -DLABCOMM_COMPAT=\"labcomm_compat_osx.h\" \
	 -Wno-tautological-compare -Wno-unused-function
  LDFLAGS=-L.
  LDLIBS=-llabcomm
  MAKESHARED=clang -o $1 -shared -Wl,-install_name,$2 $3 -lc
else
  $(error Unknown system $(UNAME_S))
endif
# TODO: How to handle Cygwin?

ifeq ($(CROSS_COMPILE),i586-wrs-vxworks-)
  ALL_DEPS:=$(filter-out %.so.1, $(ALL_DEPS)) # PIC is only supported for RTPs
  CFLAGS:=$(CFLAGS) -DLABCOMM_COMPAT=\"labcomm_compat_vxworks.h\"
endif
# TODO: Support for Codesourcery ARM toolchain.

OBJS=labcomm_memory.o \
     labcomm_error.o \
     labcomm_default_error_handler.o \
     labcomm_default_memory.o \
     labcomm_default_scheduler.o \
     labcomm_time.o labcomm_scheduler.o \
     labcomm_encoder.o labcomm_decoder.o \
     labcomm.o \
     labcomm_dynamic_buffer_writer.o \
     labcomm_fd_reader.o labcomm_fd_writer.o \
     labcomm_pthread_scheduler.o 

#FIXME: labcomm_mem_reader.o labcomm_mem_writer.o
LABCOMM_JAR=../../compiler/labComm.jar
LABCOMM=java -jar $(LABCOMM_JAR) 

TESTS=test_labcomm_basic_type_encoding test_labcomm_generated_encoding \
      test_signature_numbers \
      test_labcomm \
      test_labcomm_pthread_scheduler \
      test_labcomm_copy
#
#FIXME: test_labcomm test_labcomm_errors
TEST_DIR=test

VPATH=$(TEST_DIR)

# Enable experimental objects by invoking make like `make LABCOMM_EXPERIMENTAL=true`
ifeq ($(LABCOMM_EXPERIMENTAL),true)
	OBJS += experimental/udp_hack.o experimental/ethaddr.o \
		experimental/labcomm_thr_reader_writer.o \
		experimental/ThrottleDrv/ethernet_drv.o \
		experimental/ThrottleDrv/throttle_drv.o \
		experimental/labcomm_udp_reader_writer.o
endif

## Targets

.PHONY: all run-test clean distclean

all: $(ALL_DEPS)

liblabcomm.a: $(OBJS)
	ar -r $@ $^

liblabcomm.so.1: $(OBJS:%.o=%.pic.o)
	$(call MAKESHARED,$@,$@,$^)

labcomm.o : labcomm.c labcomm.h  labcomm_private.h

labcomm_fd_reader_writer.o : labcomm_fd_reader_writer.c  labcomm_fd_reader_writer.h  labcomm.h  labcomm_private.h

#$(TEST_DIR)/labcomm_mem_reader.o: labcomm_fd_reader_writer.c labcomm_fd_reader_writer.h

#$(TEST_DIR)/labcomm_mem_writer.o: labcomm_mem_writer.c labcomm_mem_writer.h cppmacros.h

ethaddr.o: ethaddr.c

run-test: $(TESTS:%=run-test-%)

run-test-%: $(TEST_DIR)/% | $(TEST_DIR)
	$<

$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PRECIOUS: $(TEST_DIR)/%
$(TEST_DIR)/%: $(TEST_DIR)/%.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) 

$(TEST_DIR)/gen:
	mkdir -p $@

.PRECIOUS: $(TEST_DIR)/gen/%.c
.PRECIOUS: $(TEST_DIR)/gen/%.h
$(TEST_DIR)/gen/%.c $(TEST_DIR)/gen/%.h: $(TEST_DIR)/%.lc | $(TEST_DIR)/gen
	$(LABCOMM) \
		--c=$(TEST_DIR)/gen/$*.c \
		--h=$(TEST_DIR)/gen/$*.h \
		$<

$(TEST_GEN_DIR)/%.c $(TEST_GEN_DIR)/%.h: $(TESTDATA_DIR)/%.lc \
					 $(LABCOMM_JAR) | $(TEST_GEN_DIR)
	$(LABCOMM) \
		--c=$(patsubst %.h,%.c,$@) --h=$(patsubst %.c,%.h,$@) $<

$(LABCOMM_JAR):
	@echo "======Building LabComm compiler======"
	cd $(shell dirname $(LABCOMM_JAR)); ant jar
	@echo "======End building LabComm compiler======"

%.pic.o:	%.c
	$(CC) -fPIC $(CFLAGS) -c -o $@ $<

%.o: %.c %.h

clean:
	$(RM) *.o
	$(RM) experimental/*.o experimental/ThrottleDrv/*.o
	$(RM) test/*.o
	$(RM) test/*.gch
	$(RM) test/test_labcomm_errors
	$(RM) test/testdata/gen/*.[cho]
	$(RM) test/gen/*.[cho]
	$(RM) $(TEST_DIR)/test_labcomm
	$(RM) $(TEST_DIR)/test_labcomm_copy

distclean: clean
	$(RM) liblabcomm.so.1
	$(RM) liblabcomm.a

# Extra dependencies
$(TEST_DIR)/test_labcomm_basic_type_encoding.o: labcomm_private.h

$(TEST_DIR)/test_labcomm_generated_encoding.o: labcomm_private.h
$(TEST_DIR)/test_labcomm_generated_encoding.o: $(TEST_DIR)/gen/generated_encoding.h
$(TEST_DIR)/test_labcomm_generated_encoding : $(TEST_DIR)/gen/generated_encoding.o

$(TEST_DIR)/test_labcomm:  $(TEST_DIR)/gen/test_sample.o
$(TEST_DIR)/test_signature_numbers.c: $(TEST_DIR)/gen/another_encoding.h
$(TEST_DIR)/test_signature_numbers.c: $(TEST_DIR)/gen/generated_encoding.h
$(TEST_DIR)/test_signature_numbers: $(TEST_DIR)/gen/another_encoding.o
$(TEST_DIR)/test_signature_numbers: $(TEST_DIR)/gen/generated_encoding.o
$(TEST_DIR)/test_labcomm_copy:  $(TEST_DIR)/gen/generated_encoding.o $(TEST_DIR)/gen/test_sample.o $(TEST_DIR)/gen/more_types.o
labcomm_fd_reader.o: labcomm_private.h
labcomm_fd_writer.o: labcomm_private.h
labcomm_dynamic_buffer_writer.o: labcomm_private.h
