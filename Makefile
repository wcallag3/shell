all: shell

shell:
	gcc -std=gnu90 -Wall functions.c shell.c -o shell

clean: 
	rm shell
