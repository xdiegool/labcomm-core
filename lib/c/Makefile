CC = gcc
CFLAGS = -g

liblabcomm.a : labcomm.o labcomm_fd_reader_writer.o ethaddr.o
	ar -r liblabcomm.a labcomm.o labcomm_fd_reader_writer.o ethaddr.o

labcomm.o : labcomm.c labcomm.h  labcomm_private.h

labcomm_fd_reader_writer.o : labcomm_fd_reader_writer.c  labcomm_fd_reader_writer.h  labcomm.h  labcomm_private.h

ethaddr.o: ethaddr.c

clean:
	rm *.o

distclean: clean
	rm liblabcomm.a
