#gcc -o thr_encoder -L ../../lib/c -I . -I ../../lib/c thr_encoder.c gen/simple.c ../../lib/c/labcomm_thr_reader_writer.c ../../lib/c/ThrottleDrv/*.c -llabcomm

#gcc -o thr_decoder -L ../../lib/c -I . -I ../../lib/c thr_decoder.c gen/simple.c ../../lib/c/labcomm_thr_reader_writer.c ../../lib/c/ThrottleDrv/*.c -llabcomm

gcc -o thr_example -L ../../lib/c -I . -I ../../lib/c thr_example.c gen/simple.c ../../lib/c/labcomm_thr_reader_writer.c ../../lib/c/ThrottleDrv/*.c -llabcomm
