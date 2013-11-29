LCDIR=../..
LCC=java -jar ${LCDIR}/compiler/labComm.jar
CLASSPATH=.:${LCDIR}/lib/java/labcomm.jar
JAVA_PKG=labcommTCPtest

${JAVA_PKG}/gen/FooSample.java: test.lc
	${LCC} --javapackage=${JAVA_PKG}.gen --java=${JAVA_PKG}/gen $<

${JAVA_PKG}/gen/FooSample.class: ${JAVA_PKG}/gen/FooSample.java
	javac -cp ${CLASSPATH} $<

${JAVA_PKG}/server/TestServer.class: ${JAVA_PKG}/server/TestServer.java ${JAVA_PKG}/gen/FooSample.class
	javac -cp ${CLASSPATH} $<

${JAVA_PKG}/server/OneShotServer.class: ${JAVA_PKG}/server/OneShotServer.java ${JAVA_PKG}/gen/FooSample.class
	javac -cp ${CLASSPATH} $<

${JAVA_PKG}/client/TestClient.class: ${JAVA_PKG}/client/TestClient.java ${JAVA_PKG}/gen//FooSample.class
	javac -cp ${CLASSPATH} $<

.PHONY : clean runserver runclient

runserver : ${JAVA_PKG}/server/TestServer.class
	java -cp ${CLASSPATH} $(<:.class=)

runOSserver : ${JAVA_PKG}/server/OneShotServer.class
	java -cp ${CLASSPATH} $(<:.class=)

runclient : ${JAVA_PKG}/client/TestClient.class
	java -cp ${CLASSPATH} $(<:.class=)

clean :
	rm  ${JAVA_PKG}/server/*.class ${JAVA_PKG}/client/*.class ${JAVA_PKG}/gen/* 
