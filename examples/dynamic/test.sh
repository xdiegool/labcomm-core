#dummy script to test the on-the-fly compilation

javac -cp .:../../compiler/labcomm2006_compiler.jar:../../lib/java/labcomm.jar:../../lib/tools/beaver.jar:../../lib/tools/beaver-rt.jar:../../lib/tools/jastadd2.jar:../../lib/tools/JFlex.jar:../../lib/tools/proj.jar  test/TestLabcommGen.java 

javac test/HandlerContext.java

java -cp .:../../compiler/labcomm2006_compiler.jar:../../lib/java/labcomm.jar:../../lib/tools/beaver.jar:../../lib/tools/beaver-rt.jar:../../lib/tools/jastadd2.jar:../../lib/tools/JFlex.jar:../../lib/tools/proj.jar  test.TestLabcommGen simple.lc handlers2.txt encoded_data
