all: exec

exec: looper.c LineParser.c LineParser.h
	gcc -g -m32 -c -o looper.o looper.c
	gcc -g -m32 -c -o LineParser.o LineParser.c
	gcc -g -m32  looper.o LineParser.o -o looper

LineParser: LineParser.c LineParser.h
	gcc -g -m32 -c -o LineParser.o LineParser.c



.PHONY: clean
clean:
	rm -rf ./*.o looper

