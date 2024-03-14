CC=gcc
CFLAGS=-Wall -std=c17 -g

#wpisac w termianu aby shared library dzialalo
#export LD_LIBRARY_PATH=/home/user/shared:$LD_LIBRARY_PATH

.PHONY: clean

all: main testStatic testShared testDynamic
	@echo "Compiling everything"

testDynamic: libdynamic.so
	@echo "Compliing dynamic library"
	$(CC) $(CFLAGS) testDynamic.c -o testDynamic -ldl

main: libStatyczna.a
	@echo "Compiling main with static library"
	$(CC) $(CFLAGS) main.c -o main -L. -lStatyczna

testStatic: libStatyczna.a
	@echo "Compiling testStatic with static library"
	$(CC) $(CFLAGS) testStatic.c -o testStatic -L. -lStatyczna

testShared: libshared.so
	@echo "Compiling testShared with shared library"
	gcc -L. -Wall -o testShared testShared.c -lshared

statyczna.o: statyczna.c
	gcc -c statyczna.c -o statyczna.o

libStatyczna.a: statyczna.o
	@echo "Compiling static library"
	ar rcs libStatyczna.a statyczna.o

shared.o: shared.c
	gcc -c -Wall -Werror -fpic shared.c

libshared.so: shared.o
	@echo "Compiling a shared library"
	gcc -shared -o libshared.so shared.o

dynamic.o: dynamic.c
	gcc -c -fPIC dynamic.c -o dynamic.o

libdynamic.so: dynamic.o
	gcc -shared -o libdynamic.so dynamic.o


	
clean:
	@echo "Removing all binary files"
	rm -f main testStatic testShared testDynamic statyczna.o libStatyczna.a libshared.so shared.o dynamic.o libdynamic.so

#wpisac w termianu aby shared library dzialalo
#export LD_LIBRARY_PATH=/home/user/shared:$LD_LIBRARY_PATH
