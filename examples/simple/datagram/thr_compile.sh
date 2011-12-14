(cd ../../../lib/c; make)
gcc -o thr_example -L ../../../lib/c -I . -I .. -I ../../../lib/c thr_example.c ../gen/simple.c  -llabcomm
#gcc -o thr_example -L ../../../lib/c -I . -I .. -I ../../../lib/c thr_example.c ../gen/simple.c ../../../lib/c/labcomm_thr_reader_writer.c ../../../lib/c/ThrottleDrv/*.c -llabcomm
