# Makefile for SWISS
# 
# April 2013 - Bryant Moscon


swiss: main.o
	g++ -g -Wall -o swiss main.o -lpthread -ldl

main.o: main.cc sserver.hpp module_manager.hpp thread_pool/thread_pool.hpp
	g++ -g -Wall -c main.cc 

clean:
	rm swiss main.o
