#(cd ../../lib/c; make -e LABCOMM_NO_EXPERIMENTAL=true)
(cd ../../compiler ; ant jar)

mkdir -p gen
java -jar ../../compiler/labComm.jar --java=gen --c=gen/simple.c --h=gen/simple.h  --python=gen/simple.py simple.lc 

javac -cp ../../lib/java:. gen/*.java Encoder.java Decoder.java

gcc -Wall -Werror -I . -I ../../lib/c \
    -DLABCOMM_FD_OMIT_VERSION \
    -DLABCOMM_ENCODER_LINEAR_SEARCH \
     gen/simple.c  ../../lib/c/labcomm.c \
    ../../lib/c/labcomm_dynamic_buffer_writer.c \
    ../../lib/c/labcomm_fd_writer.c \
    -o example_encoder example_encoder.c 
gcc -Wall -Werror -I . -I ../../lib/c \
    -DLABCOMM_FD_OMIT_VERSION \
    -DLABCOMM_ENCODER_LINEAR_SEARCH \
     gen/simple.c  ../../lib/c/labcomm.c \
    ../../lib/c/labcomm_dynamic_buffer_writer.c \
    ../../lib/c/labcomm_fd_reader.c \
    -o example_decoder example_decoder.c

#gcc -o example_encoder -I . -I ../../lib/c example_encoder.c gen/simple.c ../../lib/c/labcomm.c ../../lib/c/labcomm_fd_reader_writer.c

#gcc -o example_decoder -I . -I ../../lib/c example_decoder.c gen/simple.c ../../lib/c/labcomm.c ../../lib/c/labcomm_fd_reader_writer.c


