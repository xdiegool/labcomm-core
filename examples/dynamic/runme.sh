#dummy script to test the on-the-fly compilation

javac -cp .:../../compiler/labComm.jar:../../lib/java/labcomm.jar:../../lib/tools/beaver.jar:../../lib/tools/beaver-rt.jar:../../lib/tools/jastadd2.jar:../../lib/tools/JFlex.jar:../../lib/tools/proj.jar  TestLabcommGen.java

java -cp .:../../compiler/labComm.jar:../../lib/java/labcomm.jar:../../lib/tools/beaver.jar:../../lib/tools/beaver-rt.jar:../../lib/tools/jastadd2.jar:../../lib/tools/JFlex.jar:../../lib/tools/proj.jar  TestLabcommGen simple.lc handlers.txt encoded_data
