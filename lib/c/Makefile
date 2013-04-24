## Macros

# Use LLVM clang if it's found.
CC = $(shell hash clang 2>/dev/null && echo clang || echo gcc)
CFLAGS = -g -Wall -Werror -O3  -I. -Itest -DLABCOMM_ENCODER_LINEAR_SEARCH
LDFLAGS = -L.
LDLIBS_TEST = -Tlabcomm.linkscript  -lcunit -llabcomm

OBJS= labcomm.o labcomm_dynamic_buffer_writer.o labcomm_fd_reader.o labcomm_fd_writer.o labcomm_mem_reader.o labcomm_mem_writer.o
LABCOMMC_PATH=../../compiler
LABCOMMC_JAR=$(LABCOMMC_PATH)/labComm.jar

TESTS=test_labcomm_basic_type_encoding test_labcomm test_labcomm_errors
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

all: liblabcomm.a test/test_labcomm_errors

liblabcomm.a: $(OBJS)
	ar -r liblabcomm.a $^


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

$(TEST_GEN_DIR)/%.c $(TEST_GEN_DIR)/%.h: $(TESTDATA_DIR)/%.lc \
					 $(LABCOMMC_JAR) | $(TEST_GEN_DIR)
	java -jar $(LABCOMMC_JAR) \
		--c=$(patsubst %.h,%.c,$@) --h=$(patsubst %.c,%.h,$@) $<

$(LABCOMMC_JAR):
	@echo "======Building LabComm compiler======"
	cd $(LABCOMMC_PATH); ant jar
	@echo "======End building LabComm compiler======"

%.o: %.c %.h

clean:
	$(RM) *.o
	$(RM) experimental/*.o experimental/ThrottleDrv/*.o
	$(RM) test/*.o
	$(RM) test/*.gch
	$(RM) test/test_labcomm_errors
	$(RM) test/testdata/gen/*.[cho]
	$(RM) $(TEST_DIR)/test_labcomm

distclean: clean
	$(RM) liblabcomm.a

# Extra dependencies
$(TEST_DIR)/test_labcomm:  $(TEST_GEN_DIR)/test_sample.o
$(TEST_DIR)/test_labcomm_private.o: labcomm_private.h
