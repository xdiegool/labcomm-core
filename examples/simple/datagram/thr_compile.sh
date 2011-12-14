#(cd ../../../lib/c; make)
(cd ..; sh compile.sh)

gcc -o thr_example -L ../../../lib/c -I . -I .. -I ../../../lib/c -I ../../../lib/c/experimental thr_example.c ../gen/simple.c  -llabcomm
