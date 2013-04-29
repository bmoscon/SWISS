#
# Swiss Module Library
#
# Bryant Moscon - April 2013
#

libswissmod.a: module_lib.c module_lib.h
	gcc -fPIC -c -Wall -g -o libswissmod.o module_lib.c
	ar rcsv libswissmod.a libswissmod.o

clean:
	rm libswissmod.a libswissmod.o
