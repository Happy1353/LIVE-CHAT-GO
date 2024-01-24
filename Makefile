program: src/lexemes.c src/list.c src/tree.c src/main.c src/exec.c
	gcc -o 1 -g  src/lexemes.c src/list.c src/tree.c src/main.c src/exec.c
clean:
	rm -f 1
