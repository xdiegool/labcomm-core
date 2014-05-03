### Example compile script, showing the steps required to build a labcomm application
### (including compiler and libs). Also illustrates how versions 2013 and 2006 coexist

# For current version (2013)
(cd ../..; make all)

mkdir -p gen
java -jar ../../compiler/labComm.jar --java=gen --c=gen/simple.c --h=gen/simple.h  --python=gen/simple.py simple.lc 

javac -cp ../../lib/java:. gen/*.java Encoder.java Decoder.java

gcc -Wall -Werror -Wno-unused-function \
    -I. -I../../lib/c -L../../lib/c \
    -o example_encoder example_encoder.c gen/simple.c \
    -llabcomm2013 -Tlabcomm.linkscript
gcc -Wall -Werror -I . -I ../../lib/c -L../../lib/c \
    -o example_decoder example_decoder.c gen/simple.c \
    -llabcomm2013 -Tlabcomm.linkscript

# For version 2006

mkdir -p gen06
java -jar ../../compiler/labComm.jar --ver=2006 --java=gen06 --c=gen06/simple.c --h=gen06/simple.h  --python=gen06/simple.py simple.lc 

javac -cp ../../lib/java:. gen06/*.java Encoder06.java Decoder06.java

gcc -Wall -Werror -I.  -I../../lib/c/2006 -L../../lib/c \
    -o example_encoder06 example_encoder06.c gen06/simple.c \
    -llabcomm -Tlabcomm.linkscript
gcc -Wall -Werror -I . -I ../../lib/c/2006 -L../../lib/c \
    -o example_decoder06 example_decoder06.c gen06/simple.c \
    -llabcomm -Tlabcomm.linkscript

#gcc -o example_encoder -I . -I ../../lib/c example_encoder.c gen/simple.c ../../lib/c/labcomm.c ../../lib/c/labcomm_fd_reader_writer.c

#gcc -o example_decoder -I . -I ../../lib/c example_decoder.c gen/simple.c ../../lib/c/labcomm.c ../../lib/c/labcomm_fd_reader_writer.c


