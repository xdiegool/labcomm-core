## Macros
VERSION=2014
LIBVERSION=2014

include ../os_compat.mk

ALL_DEPS=../liblabcomm$(LIBVERSION).a ../liblabcomm$(LIBVERSION).so

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
     labcomm$(VERSION)_type_signature.o \
     labcomm$(VERSION)_fd_writer.o \
     labcomm$(VERSION)_pthread_scheduler.o \
     labcomm$(VERSION)_renaming.o \
     labcomm$(VERSION)_renaming_encoder.o \
     labcomm$(VERSION)_renaming_decoder.o

# Enable experimental objects by `make LABCOMM_EXPERIMENTAL=true`
ifeq ($(LABCOMM_EXPERIMENTAL),true)
    OBJS += experimental/udp_hack.o experimental/ethaddr.o \
	    experimental/labcomm_thr_reader_writer.o \
	    experimental/ThrottleDrv/ethernet_drv.o \
	    experimental/ThrottleDrv/throttle_drv.o \
	    experimental/labcomm_udp_reader_writer.o
endif

# Enable experimental objects by `make LABCOMM_SIG_PARSER=true`
ifeq ($(LABCOMM_SIG_PARSER),true)
    OBJS += experimental/labcomm_sig_parser.o 
endif

LABCOMM_JAR=../../../compiler/labcomm$(LIBVERSION)_compiler.jar
LABCOMM=java -jar $(LABCOMM_JAR) 

TESTS=test_labcomm_basic_type_encoding \
      test_labcomm_generated_encoding \
      test_signature_numbers \
      test_labcomm \
      test_labcomm_pthread_scheduler \
      test_labcomm_copy \
      test_labcomm_renaming_encoder
#FIXME: test_labcomm_errors

TEST_DIR=test


## Targets

.PHONY: all 
all: $(ALL_DEPS)

.PHONY: test 
test: all $(TESTS:%=run-test-%)

.PHONY: clean 
clean:
	$(RM) *.o
	$(RM) experimental/*.o 
	$(RM) experimental/ThrottleDrv/*.o
	$(RM) test/*.o
	$(RM) test/*.gch
	$(RM) test/test_labcomm_errors
	$(RM) test/testdata/gen/*.[cho]
	$(RM) test/gen/*.[cho]
	$(RM) -rf test/gen

.PHONY: distclean
distclean: clean
	$(RM) ../liblabcomm$(LIBVERSION).so.1
	$(RM) ../liblabcomm$(LIBVERSION).a

# rules invoked by 'all'
../liblabcomm$(LIBVERSION).so: ../liblabcomm$(LIBVERSION).so.1
	if [ -h $@ ] ; then rm $@ ; fi
	ln -s $(<:../%=%) $@

../liblabcomm$(LIBVERSION).so.1: $(OBJS:%.o=%.pic.o)
	$(call MAKESHARED,$@,$(@:../%=%),$^)

../liblabcomm$(LIBVERSION).a: $(OBJS)
	ar -r $@ $^

# Enable sig parser objects by `make LABCOMM_SIG_PARSER=true`
ifeq ($(LABCOMM_SIG_PARSER),true)
experimental/test_sig_parser : experimental/labcomm_sig_parser.o experimental/test_sig_parser.c
endif

# compilation rules 
%.pic.o:	%.c
	$(CC) -fPIC $(CFLAGS) -c -o $@ $<

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

# rules invoked by 'test'
.PHONY: run-test-%
run-test-%: $(TEST_DIR)/gen/% | $(TEST_DIR)/gen
	$<

.PRECIOUS: $(TEST_DIR)/gen/%

$(TEST_DIR)/gen/%: $(TEST_DIR)/gen/%.o | $(TEST_DIR)/gen
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS) 

$(TEST_DIR)/gen/%.o: $(TEST_DIR)/%.c | $(TEST_DIR)/gen
	$(CC) $(CFLAGS_TEST) -o $@ -c $<

.PRECIOUS: $(TEST_DIR)/gen/%.c
.PRECIOUS: $(TEST_DIR)/gen/%.h
$(TEST_DIR)/gen/%.c $(TEST_DIR)/gen/%.h: $(TEST_DIR)/%.lc | $(TEST_DIR)/gen
	$(LABCOMM) \
		--c=$(TEST_DIR)/gen/$*.c \
		--h=$(TEST_DIR)/gen/$*.h \
		$<

$(LABCOMM_JAR):
	@echo "======Building LabComm compiler======"
	cd $(shell dirname $(LABCOMM_JAR)); ant jar
	@echo "======End building LabComm compiler======"

$(TEST_DIR)/gen:
	mkdir -p $@

# Extra compilation dependencies
labcomm$(VERSION).o: \
	labcomm$(VERSION).c \
	labcomm$(VERSION).h \
	labcomm$(VERSION)_private.h

labcomm$(VERSION)_fd_reader.o: \
	labcomm$(VERSION)_private.h

labcomm$(VERSION)_fd_writer.o: \
	labcomm$(VERSION)_private.h

labcomm$(VERSION)_dynamic_buffer_writer.o: \
	labcomm$(VERSION)_private.h

$(TEST_DIR)/gen/test_labcomm_basic_type_encoding.o: \
	labcomm$(VERSION)_private.h

$(TEST_DIR)/gen/test_labcomm_generated_encoding.o: \
	labcomm$(VERSION)_private.h \
	$(TEST_DIR)/gen/generated_encoding.h

$(TEST_DIR)/gen/test_labcomm_generated_encoding: \
	$(TEST_DIR)/gen/generated_encoding.o

$(TEST_DIR)/gen/test_signature_numbers.c: \
	$(TEST_DIR)/gen/another_encoding.h \
	$(TEST_DIR)/gen/generated_encoding.h				

$(TEST_DIR)/gen/test_signature_numbers: \
	$(TEST_DIR)/gen/another_encoding.o \
	$(TEST_DIR)/gen/generated_encoding.o

$(TEST_DIR)/gen/test_labcomm:  \
	$(TEST_DIR)/gen/test_sample.o

$(TEST_DIR)/gen/test_labcomm_copy: \
	$(TEST_DIR)/gen/generated_encoding.o \
	$(TEST_DIR)/gen/test_sample.o \
	$(TEST_DIR)/gen/more_types.o

$(TEST_DIR)/gen/test_labcomm_renaming_encoder: \
	$(TEST_DIR)/gen/generated_encoding.o
