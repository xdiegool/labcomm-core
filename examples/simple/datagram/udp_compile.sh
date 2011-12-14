(cd ../../../lib/c; make)

gcc -g -o udp_encoder -L ../../../lib/c -I . -I .. -I ../../../lib/c udp_encoder.c ../gen/simple.c ../../../lib/c/labcomm_udp_reader_writer.c udp_hack.c -llabcomm

gcc -g -o udp_decoder -L ../../../lib/c -I . -I .. -I ../../../lib/c udp_decoder.c ../gen/simple.c ../../../lib/c/labcomm_udp_reader_writer.c udp_hack.c -llabcomm
