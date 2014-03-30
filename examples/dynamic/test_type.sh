#dummy script to test the on-the-fly compilation

javac -cp .:../../compiler/labComm.jar:../../lib/java/labcomm.jar:../../lib/tools/beaver.jar:../../lib/tools/beaver-rt.jar:../../lib/tools/jastadd2.jar:../../lib/tools/JFlex.jar:../../lib/tools/proj.jar  test/TestLabcommGen.java 

javac test/HandlerContext.java

java -cp .:../../compiler/labComm.jar:../../lib/java/labcomm.jar:../../lib/tools/beaver.jar:../../lib/tools/beaver-rt.jar:../../lib/tools/jastadd2.jar:../../lib/tools/JFlex.jar:../../lib/tools/proj.jar  test.TestLabcommGen simple_type.lc handlers_type.txt encoded_data
