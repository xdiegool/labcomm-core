## Macros

# Use LLVM clang if it's found.
CC = $(shell hash clang 2>/dev/null && echo clang || echo gcc)
CFLAGS = -g -Wall -I .
LDFLAGS = -L .
LDLIBS_TEST = -lcunit -llabcomm

OBJS= labcomm.o labcomm_fd_reader_writer.o labcomm_mem_reader.o labcomm_mem_writer.o
LABCOMMC_PATH=../../compiler
LABCOMMC_JAR=$(LABCOMMC_PATH)/labComm.jar

TEST_DIR=test
TESTDATA_DIR=$(TEST_DIR)/testdata
TEST_GEN_DIR=$(TESTDATA_DIR)/gen

CREATED_DIRS=$(TEST_DIR) $(TESTDATA_DIR) $(TEST_GEN_DIR)

# Disable experimental objects by invoking make like `make -e LABCOMM_NO_EXPERIMENTAL=true`
ifneq ($(LABCOMM_NO_EXPERIMENTAL),true)
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

labcomm_mem_reader.o: labcomm_fd_reader_writer.c labcomm_fd_reader_writer.h

labcomm_mem_writer.o: labcomm_mem_writer.c labcomm_mem_writer.h cppmacros.h

ethaddr.o: ethaddr.c

$(CREATED_DIRS):
	mkdir -p $@

run-test: $(TEST_DIR)/test_labcomm_errors |$(TEST_DIR)
	test/test_labcomm_errors

$(TEST_DIR)/test_labcomm_errors: $(TEST_DIR)/test_labcomm_errors.o liblabcomm.a |$(TEST_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -llabcomm -o $@ $^

$(TEST_DIR)/test_labcomm_errors.o: $(TEST_DIR)/test_labcomm_errors.c $(TEST_DIR)/test_labcomm_errors.h |$(TEST_DIR)
	cd test; $(CC) $(CFLAGS) -I .. -c $(patsubst $(TEST_DIR)/%, %, $^)

$(TEST_DIR)/test_labcomm.o: $(TEST_DIR)/test_labcomm.c $(TEST_GEN_DIR)/test_sample.h |$(TEST_DIR)
	$(CC) -c $(CFLAGS) -o $@ $<

$(TEST_DIR)/test_labcomm: $(TEST_DIR)/test_labcomm.o $(TEST_GEN_DIR)/test_sample.o liblabcomm.a
	$(CC) $(CFLAGS) $(LDFLAGS) $(filter-out %.a,$^) $(LDLIBS) $(LDLIBS_TEST) -o $@

$(TEST_GEN_DIR)/%.c $(TEST_GEN_DIR)/%.h: $(TESTDATA_DIR)/%.lc $(LABCOMMC_JAR) |$(TEST_GEN_DIR)
	java -jar $(LABCOMMC_JAR) --c=$(patsubst %.h,%.c,$@) --h=$(patsubst %.c,%.h,$@) $<

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
	$(RM) $(TEST_DIR)/test_labcomm

distclean: clean
	$(RM) liblabcomm.a
