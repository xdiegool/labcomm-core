#dummy script to test the static part

# run labcomm compilation

java -jar ../../compiler/labComm.jar --java=gen --javapackage=gen simple.lc

# compile example programs
javac -cp .:gen:../../lib/java/labcomm.jar test/StaticEncoder.java
javac -cp .:gen:../../lib/java/labcomm.jar test/StaticDecoder.java

# run example programs
java -cp .:gen:../../lib/java//labcomm.jar test.StaticEncoder encoded_data
java -cp .:gen:../../lib/java//labcomm.jar test.StaticDecoder encoded_data
