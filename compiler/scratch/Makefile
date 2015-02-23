CLASSPATH=.:../tools/ST-4.0.8.jar
%.class : %.java
	javac -cp $(CLASSPATH) $< 

.PHONY: run 
run: StringTemplateTest.class
	java -cp $(CLASSPATH) $(<:.class=) 
