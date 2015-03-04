#!/bin/sh
set -x
set -e
### Example compile script, showing the steps required to build a labcomm application
### (including compiler and libs). Also illustrates how versions 2013 and 2006 coexist

# For current version (2013)
(cd ../..; make all)

mkdir -p gen
java -jar ../../compiler/labcomm2014_compiler.jar --java=gen --c=gen/simple.c --h=gen/simple.h  --python=gen/simple.py simple.lc 

javac -cp ../../lib/java/labcomm2014.jar:. gen/*.java Encoder.java Decoder.java

gcc -Wall -Werror -Wno-unused-function \
    -I. -I../../lib/c/2014 -L../../lib/c \
    -o example_encoder example_encoder.c gen/simple.c \
    -llabcomm2014 
gcc -Wall -Werror -I . -I ../../lib/c/2014 -L../../lib/c \
    -o example_decoder example_decoder.c gen/simple.c \
    -llabcomm2014 

