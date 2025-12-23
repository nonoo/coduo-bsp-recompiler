all:
	gcc -O3 -march=native -flto dumpbsp.c -o dumpbsp
	gcc -O3 -march=native -flto mkbsp.c -o mkbsp

clean:
	rm -f dumpbsp mkbsp
