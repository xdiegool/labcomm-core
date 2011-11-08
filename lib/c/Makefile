liblabcomm.a : labcomm.o labcomm_fd_reader_writer.o
	ar -r liblabcomm.a labcomm.o labcomm_fd_reader_writer.o

labcomm.o : labcomm.c labcomm.h  labcomm_private.h
	gcc -c labcomm.c

labcomm_fd_reader_writer.o : labcomm_fd_reader_writer.c  labcomm_fd_reader_writer.h  labcomm.h  labcomm_private.h
	gcc -c labcomm_fd_reader_writer.c

clean:
	rm *.o

distclean: clean
	rm liblabcomm.a
