#SAMPLENAME=foo
LCFILE=jg
LCDIR=../..
LCC=java -jar ${LCDIR}/compiler/labComm.jar
LCLIBDIR=${LCDIR}/lib/c
AUX=enc.c dec.c

client: client.c ${LCFILE}.c ${AUX} ${AUX:.c=.h}
	${CC} -o $@ client.c ${AUX} ${LCFILE}.c -I${LCLIBDIR} -L${LCLIBDIR} -llabcomm

testserver: testserver.c ${LCFILE}.c ${AUX} ${AUX:.c=.h}
	${CC} -o $@ testserver.c ${AUX} ${LCFILE}.c -I${LCLIBDIR} -L${LCLIBDIR} -llabcomm
	
${LCFILE}.c : ${LCFILE}.lc
	${LCC} -C ${LCFILE}.lc


.PHONY: clean runclient
clean :
	rm ${LCFILE}.c ${LCFILE}.h client testserver

runclient : client
	LD_LIBRARY_PATH=${LCLIBDIR} ./$< localhost 9999

runserver : testserver
	LD_LIBRARY_PATH=${LCLIBDIR} ./$< 9999
