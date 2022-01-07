all: parent
	./a.out

parent: child.o 
		gcc parent.c 

child.o : child.c 
		gcc child.c -o child

