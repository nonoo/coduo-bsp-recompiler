all:
	gcc -O3 -march=native -flto dumpbsp.c -o dumpbsp
	gcc -O3 -march=native -flto mkbsp.c -o mkbsp
	gcc -O3 -march=native -flto getbrushes.c -o getbrushes

clean:
	rm -f dumpbsp mkbsp getbrushes
