CC = gcc
CFLAGS = -g -I .

liblabcomm.a : labcomm.o labcomm_fd_reader_writer.o experimental/labcomm_udp_reader_writer.o \
               experimental/udp_hack.o experimental/ethaddr.o \
               experimental/labcomm_thr_reader_writer.o \
               experimental/ThrottleDrv/ethernet_drv.o experimental/ThrottleDrv/throttle_drv.o
	ar -r liblabcomm.a $^

labcomm.o : labcomm.c labcomm.h  labcomm_private.h

labcomm_fd_reader_writer.o : labcomm_fd_reader_writer.c  labcomm_fd_reader_writer.h  labcomm.h  labcomm_private.h

ethaddr.o: ethaddr.c

%o: %c %h

clean:
	rm *.o experimental/*.o experimental/ThrottleDrv/*.o

distclean: clean
	rm liblabcomm.a
