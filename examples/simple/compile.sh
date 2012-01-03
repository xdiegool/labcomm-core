(cd ../../lib/c; make)
(cd ../../compiler ; ant jar)

java -jar ../../compiler/labComm.jar --java=gen --c=gen/simple.c --h=gen/simple.h  --python=gen/simple.py simple.lc 

javac -cp ../../lib/java:. gen/*.java Encoder.java Decoder.java

gcc -o example_encoder -L ../../lib/c -I . -I ../../lib/c example_encoder.c gen/simple.c -llabcomm
gcc -o example_decoder -L ../../lib/c -I . -I ../../lib/c example_decoder.c gen/simple.c -llabcomm

#gcc -o example_encoder -I . -I ../../lib/c example_encoder.c gen/simple.c ../../lib/c/labcomm.c ../../lib/c/labcomm_fd_reader_writer.c

#gcc -o example_decoder -I . -I ../../lib/c example_decoder.c gen/simple.c ../../lib/c/labcomm.c ../../lib/c/labcomm_fd_reader_writer.c


