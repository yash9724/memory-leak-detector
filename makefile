all:
	gcc -g -c mld.c -o mld.o
	gcc -g -c app.c -o app.o
	gcc -g -o exe mld.o app.o