CC = gcc
CFLAGS = -g

liblabcomm.a : labcomm.o labcomm_fd_reader_writer.o labcomm_udp_reader_writer.o labcomm_thr_reader_writer.o ethaddr.o ThrottleDrv/ethernet_drv.o ThrottleDrv/throttle_drv.o
#	ar -r liblabcomm.a labcomm.o labcomm_fd_reader_writer.o ethaddr.o
	ar -r liblabcomm.a $^

labcomm.o : labcomm.c labcomm.h  labcomm_private.h

labcomm_fd_reader_writer.o : labcomm_fd_reader_writer.c  labcomm_fd_reader_writer.h  labcomm.h  labcomm_private.h

ethaddr.o: ethaddr.c

%o: %c %h

clean:
	rm *.o

distclean: clean
	rm liblabcomm.a
