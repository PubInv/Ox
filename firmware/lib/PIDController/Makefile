main.o: main.cpp pidcontroller.cpp mocksimulation.cpp
	gcc -c main.cpp pidcontroller.cpp mocksimulation.cpp -I ~/Ox/firmware/lib/header -lm

mocksimulation.o: mocksimulation.o
	gcc -c mocksimulation.cpp -I ~/Ox/firmware/lib/header -lm
	

pidcontroller.o: pidcontroller.cpp
	gcc -c pidcontroller.cpp  -I ~/Ox/firmware/lib/header -lm

main: main.o pidcontroller.o mocksimulation.o
	gcc -o main.exe main.o pidcontroller.o mocksimulation.o -lm
