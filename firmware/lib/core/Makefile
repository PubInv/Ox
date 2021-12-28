all: c cpp
#g++ *.o -o boot
#	g++ -I../collections *.cpp *.c -o core

c:
	gcc -std=c99 -c HAL/posix/hal.c -lpthread

cpp:
	g++ -std=c++11 -Icollections -pthread posix.o *.cpp -o core

core:
	g++ *.cpp *.c map.o list.o queue.o -o core

collections:
	g++ -c ../collections/*.cpp

clean:
	rm *.o

.PHONY:
	clean collections core