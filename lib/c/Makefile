## Macros

CC=gcc
#CFLAGS=-g -Wall -Werror -O3  -I. -Itest -DLABCOMM_ENCODER_LINEAR_SEARCH
CFLAGS=-g -Wall -Werror -O3  -I. -Itest 
LDFLAGS=-L.
#LDLIBS_TEST=-Tlabcomm.linkscript  -lcunit -llabcomm
LDLIBS_TEST=-lcunit -llabcomm -Tlabcomm.linkscript

OBJS= labcomm.o \
      labcomm_dynamic_buffer_writer.o labcomm_fd_reader.o labcomm_fd_writer.o \
      labcomm_pthread_mutex_lock.o

#FIXME: labcomm_mem_reader.o labcomm_mem_writer.o
LABCOMM_JAR=../../compiler/labComm.jar
LABCOMM=java -jar $(LABCOMM_JAR) 

TESTS=test_labcomm_basic_type_encoding test_labcomm_generated_encoding 
#
#FIXME: test_labcomm test_labcomm_errors
TEST_DIR=test
TESTDATA_DIR=$(TEST_DIR)/testdata
TEST_GEN_DIR=$(TESTDATA_DIR)/gen

VPATH=$(TEST_DIR)

CREATED_DIRS=$(TEST_DIR) $(TESTDATA_DIR) $(TEST_GEN_DIR)

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

all: liblabcomm.a liblabcomm.so.1 

liblabcomm.a: $(OBJS)
	ar -r liblabcomm.a $^

liblabcomm.so.1: $(OBJS:%.o=%.pic.o)
	gcc -shared -Wl,-soname,$@ -o $@ $^ -lc

labcomm.o : labcomm.c labcomm.h  labcomm_private.h

labcomm_fd_reader_writer.o : labcomm_fd_reader_writer.c  labcomm_fd_reader_writer.h  labcomm.h  labcomm_private.h

#$(TEST_DIR)/labcomm_mem_reader.o: labcomm_fd_reader_writer.c labcomm_fd_reader_writer.h

#$(TEST_DIR)/labcomm_mem_writer.o: labcomm_mem_writer.c labcomm_mem_writer.h cppmacros.h

ethaddr.o: ethaddr.c

$(CREATED_DIRS):
	mkdir -p $@

## NB! the tests need CUnit to be installed
run-test: $(TESTS:%=run-test-%)

run-test-%: $(TEST_DIR)/% | $(TEST_DIR)
	$<

$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(TEST_DIR)/%: $(TEST_DIR)/%.o liblabcomm.a
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) $(LDLIBS_TEST)

$(TEST_DIR)/gen:
	mkdir -p $@

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

distclean: clean
	$(RM) liblabcomm.so.1
	$(RM) liblabcomm.a

# Extra dependencies
$(TEST_DIR)/test_labcomm_basic_type_encoding.o: labcomm_private.h

$(TEST_DIR)/test_labcomm_generated_encoding.o: labcomm_private.h
$(TEST_DIR)/test_labcomm_generated_encoding.o: $(TEST_DIR)/gen/generated_encoding.h
$(TEST_DIR)/test_labcomm_generated_encoding : $(TEST_DIR)/gen/generated_encoding.o
$(TEST_DIR)/test_labcomm:  $(TEST_GEN_DIR)/test_sample.o
labcomm_fd_reader.o: labcomm_private.h
labcomm_fd_writer.o: labcomm_private.h
labcomm_dynamic_buffer_writer.o: labcomm_private.h
