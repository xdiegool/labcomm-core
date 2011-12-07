gcc -o udp_encoder -L ../../lib/c -I . -I ../../lib/c udp_encoder.c gen/simple.c ../../lib/c/labcomm_udp_reader_writer.c ../../lib/c/udp_hack.c -llabcomm

gcc -o udp_decoder -L ../../lib/c -I . -I ../../lib/c udp_decoder.c gen/simple.c ../../lib/c/labcomm_udp_reader_writer.c ../../lib/c/udp_hack.c -llabcomm
