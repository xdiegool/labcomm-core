### Example compile script, showing the steps required to build a labcomm application
### (including compiler and libs).

# For current version (2013)
(cd ../..; make all)

mkdir -p gen
java -jar ../../compiler/labComm.jar --java=gen --c=gen/test.c --h=gen/test.h  --python=gen/test.py test.lc 

javac -cp ../../lib/java/labcomm2014.jar:. gen/*.java Encoder.java Decoder.java

# for macOS, add     -DLABCOMM_COMPAT=\"labcomm_compat_osx.h\" \
gcc -Wall -Werror -Wno-unused-function \
    -I. -I../../lib/c -L../../lib/c \
    -o example_encoder example_encoder.c gen/test.c \
    -llabcomm2014 

gcc -Wall -Werror -I . -I ../../lib/c -L../../lib/c \
    -o example_decoder example_decoder.c gen/test.c \
    -llabcomm2014 
    #-Tlabcomm.linkscript

