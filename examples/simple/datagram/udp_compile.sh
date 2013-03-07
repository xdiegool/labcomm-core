#(cd ../../../lib/c; make)

(cd ..; sh compile.sh)

gcc -g -o udp_encoder -L ../../../lib/c -I . -I .. -I ../../../lib/c -I ../../../lib/c/experimental udp_encoder.c ../gen/simple.c -llabcomm

gcc -g -o udp_decoder -L ../../../lib/c -I . -I .. -I ../../../lib/c -I ../../../lib/c/experimental udp_decoder.c ../gen/simple.c -llabcomm
