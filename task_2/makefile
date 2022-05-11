all: exec

exec: task0c.c looper.c LineParser.c 
	gcc -g -m32 -c -o LineParser.o LineParser.c
	gcc -g -m32 -c -o task0c.o task0c.c
	gcc -g -m32  task0c.o LineParser.o -o myshell


.PHONY: clean
clean:
	rm -rf ./*.o myshell

