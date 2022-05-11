all: exec

exec: task2a.c looper.c LineParser.c 
	gcc -g -m32 -c -o LineParser.o LineParser.c
	gcc -g -m32 -c -o task2a.o task2a.c
	gcc -g -m32  task2a.o LineParser.o -o myshell


.PHONY: clean
clean:
	rm -rf ./*.o myshell

