#
# Example Module Makefile
#
# Bryant Moscon - April 2013
#

example.so: example.cc
	cd ../lib; make
	g++ -fPIC -shared -I../ -L../lib/ example.cc -o example.so -lswissmod

clean:
	rm example.so
