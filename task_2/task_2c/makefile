all: task2c

looper: looper.c
	gcc -g -m32 -c -o looper.o looper.c
	gcc -g -m32  looper.o -o looper

task2c: task2c.c LineParser.c
	gcc -g -m32 -c -o task2c.o task2c.c
	gcc -g -m32 -c -o LineParser.o LineParser.c
	gcc -g -m32  task2c.o LineParser.o -o myshell
	

.PHONY: clean
clean:
	rm -rf ./*.o myshell

