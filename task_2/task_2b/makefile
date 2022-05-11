all: task2b

looper: looper.c
	gcc -g -m32 -c -o looper.o looper.c
	gcc -g -m32  looper.o -o looper

task2b: task2b.c LineParser.c
	gcc -g -m32 -c -o task2b.o task2b.c
	gcc -g -m32 -c -o LineParser.o LineParser.c
	gcc -g -m32  task2b.o LineParser.o -o myshell
	

.PHONY: clean
clean:
	rm -rf ./*.o myshell

