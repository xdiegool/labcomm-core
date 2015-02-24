CLASSPATH=.:../tools/ST-4.0.8.jar

.PHONY: all

all: run

%.class : %.java
	javac -cp $(CLASSPATH) $< 

.PHONY: run 
run: StringTemplateTest.class
	java -cp $(CLASSPATH) $(<:.class=) 
