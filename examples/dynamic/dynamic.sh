#dummy script to test the on-the-fly compilation

java -jar ../../compiler/labcomm_compiler.jar --java=gen --javapackage=gen simple.lc

javac -cp .:gen:../../lib/java/labcomm.jar gen/*.java

# compile static encoder and decoder 
javac -cp .:gen:../../lib/java/labcomm.jar test/StaticEncoder.java
javac -cp .:gen:../../lib/java/labcomm.jar test/StaticDecoder.java 

# compile dynamic part 
javac -cp .:../../compiler/labcomm_compiler.jar:../../lib/java/labcomm.jar:../../lib/tools/beaver.jar:../../lib/tools/beaver-rt.jar:../../lib/tools/jastadd2.jar:../../lib/tools/JFlex.jar:../../lib/tools/proj.jar  test/DynamicPart.java 

javac test/HandlerContext.java

# run static encoder 
java -cp .:gen:../../lib/java//labcomm.jar test.StaticEncoder encoded_data

# run dynamic part 
java -cp .:../../compiler/labcomm_compiler.jar:../../lib/java/labcomm.jar:../../lib/tools/beaver.jar:../../lib/tools/beaver-rt.jar:../../lib/tools/jastadd2.jar:../../lib/tools/JFlex.jar:../../lib/tools/proj.jar  test.DynamicPart simple.lc handlers2.txt encoded_data dynamic_out


# run static decoder 
java -cp .:gen:../../lib/java//labcomm.jar test.StaticDecoder dynamic_out
