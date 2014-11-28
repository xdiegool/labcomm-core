## Macros
VERSION=
LIBVERSION=2014

include ../os_compat.mk

ALL_DEPS=../liblabcomm$(LIBVERSION).a ../liblabcomm$(LIBVERSION).so.1

# TODO: Support for Codesourcery ARM toolchain.

OBJS=labcomm$(VERSION).o \
     labcomm$(VERSION)_memory.o \
     labcomm$(VERSION)_error.o \
     labcomm$(VERSION)_default_error_handler.o \
     labcomm$(VERSION)_default_memory.o \
     labcomm$(VERSION)_default_scheduler.o \
     labcomm$(VERSION)_time.o \
     labcomm$(VERSION)_scheduler.o \
     labcomm$(VERSION)_encoder.o \
     labcomm$(VERSION)_decoder.o \
     labcomm$(VERSION)_dynamic_buffer_writer.o \
     labcomm$(VERSION)_fd_reader.o \
     labcomm$(VERSION)_fd_writer.o \
     labcomm$(VERSION)_pthread_scheduler.o 

#FIXME: labcomm_mem_reader.o labcomm_mem_writer.o
LABCOMM_JAR=../../../compiler/labcomm_compiler.jar
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
		experimental/labcomm_udp_reader_writer.o\
		experimental/labcomm_sig_parser.o 


experimental/test_sig_parser : experimental/labcomm_sig_parser.o experimental/test_sig_parser.c
endif

## Targets

.PHONY: all run-test clean distclean

all: $(ALL_DEPS)

test: all $(TESTS:%=run-test-%)

clean:
	$(RM) *.o
	$(RM) experimental/*.o 
	$(RM) experimental/ThrottleDrv/*.o
	$(RM) test/*.o
	$(RM) test/*.gch
	$(RM) test/test_labcomm_errors
	$(RM) test/testdata/gen/*.[cho]
	$(RM) test/gen/*.[cho]
	-rmdir test/gen
	for x in $(TESTS); do \
		$(RM) $(TEST_DIR)/$$x ; \
	done

distclean: clean
	$(RM) ../liblabcomm$(LIBVERSION).so.1
	$(RM) ../liblabcomm$(LIBVERSION).a


# rules invoked by 'all'
../liblabcomm$(LIBVERSION).a: $(OBJS)
	ar -r $@ $^

../liblabcomm$(LIBVERSION).so.1: $(OBJS:%.o=%.pic.o)
	$(call MAKESHARED,$@,$@,$^)

# compilation rules 
%.pic.o:	%.c
	$(CC) -fPIC $(CFLAGS) -c -o $@ $<

%.o: %.c %.h

# rules invoked by 'test'

# Extra compilation dependencies
labcomm$(VERSION).o: \
	labcomm$(VERSION).c \
	labcomm$(VERSION).h \
	labcomm$(VERSION)_private.h

labcomm$(VERSION)_fd_reader_writer.o: \
	labcomm$(VERSION)_fd_reader_writer.c \
	labcomm$(VERSION)_fd_reader_writer.h  \
	labcomm$(VERSION).h \
	labcomm$(VERSION)_private.h

labcomm.o : labcomm.c labcomm.h  labcomm_private.h

labcomm_fd_reader_writer.o : labcomm_fd_reader_writer.c  labcomm_fd_reader_writer.h  labcomm.h  labcomm_private.h

2006/labcomm2006.o : 2006/labcomm2006.c 2006/labcomm2006.h  2006/labcomm2006_private.h

2006/labcomm2006_fd_reader_writer.o : 2006/labcomm2006_fd_reader_writer.c  2006/labcomm2006_fd_reader_writer.h  labcomm.h  2006/labcomm2006_private.h

#$(TEST_DIR)/labcomm_mem_reader.o: labcomm_fd_reader_writer.c labcomm_fd_reader_writer.h

#$(TEST_DIR)/labcomm_mem_writer.o: labcomm_mem_writer.c labcomm_mem_writer.h cppmacros.h

ethaddr.o: ethaddr.c

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
