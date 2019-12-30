p1: main.o proj1.o builtIns.o helpers.o
	gcc -g -o p1 main.o proj1.o builtIns.o helpers.o

main.o: main.c
	gcc -c -g main.c

proj1.o: proj1.c
	gcc -c -g proj1.c

builtIns.o: builtIns.c
	gcc -c -g builtIns.c

helpers.o: helpers.c
	gcc -c -g helpers.c

clean: 
	rm *.o 
