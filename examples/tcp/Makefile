LCDIR=../..
LABCOMM_JAR=../../compiler/labcomm2014_compiler.jar
LABCOMM=java -jar $(LABCOMM_JAR)
LCLJAR=${LCDIR}/lib/java/labcomm2014.jar  # the LabComm library
JAVA_PKG=labcommTCPtest

.PHONY : clean run runserver runOSserver runclient

#### The main example #########################

run : ${JAVA_PKG}/Example.class ${JAVA_PKG}/server/TestServer.class ${JAVA_PKG}/client/TestClient.class
	java -cp ${CLASSPATH} $(<:.class=)

###############################################
### dependencies and parts ####################
###############################################

CLASSPATH=.:${LCLJAR}

${LABCOMM_JAR} :
	cd ${LCDIR} && make make-compiler

${LCLJAR} :
	cd ${LCDIR}/lib/java && make labcomm2014.jar

${JAVA_PKG}/gen/FooSample.java: test.lc ${LCCJAR}
	${LABCOMM} --javapackage=${JAVA_PKG}.gen --java=${JAVA_PKG}/gen $<

${JAVA_PKG}/gen/FooSample.class: ${JAVA_PKG}/gen/FooSample.java test.lc ${LCLJAR}   
	javac -cp ${CLASSPATH} $<

${JAVA_PKG}/Example.class: ${JAVA_PKG}/Example.java ${JAVA_PKG}/gen/FooSample.class ${LCLJAR} 
	javac -cp ${CLASSPATH} $<

${JAVA_PKG}/server/TestServer.class: ${JAVA_PKG}/server/TestServer.java ${JAVA_PKG}/gen/FooSample.class ${LCLJAR} 
	javac -cp ${CLASSPATH} $<

${JAVA_PKG}/server/OneShotServer.class: ${JAVA_PKG}/server/OneShotServer.java ${JAVA_PKG}/gen/FooSample.class ${LCLJAR} 
	javac -cp ${CLASSPATH} $<

${JAVA_PKG}/client/TestClient.class: ${JAVA_PKG}/client/TestClient.java ${JAVA_PKG}/gen//FooSample.class ${LCLJAR} 
	javac -cp ${CLASSPATH} $<

runserver : ${JAVA_PKG}/server/TestServer.class
	java -cp ${CLASSPATH} $(<:.class=)

runOSserver : ${JAVA_PKG}/server/OneShotServer.class
	java -cp ${CLASSPATH} $(<:.class=)

runclient : ${JAVA_PKG}/client/TestClient.class
	java -cp ${CLASSPATH} $(<:.class=)

clean :
	rm -f ${JAVA_PKG}/server/*.class 
	rm -f ${JAVA_PKG}/client/*.class 
	rm -f ${JAVA_PKG}/gen/* 

distclean: clean
