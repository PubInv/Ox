main.o: main.cpp pidcontroller.cpp mocksimulation.cpp
	g++ -c main.cpp pidcontroller.cpp mocksimulation.cpp -I ~/Ox/firmware/lib/header -lm

mocksimulation.o: mocksimulation.cpp
	g++ -c mocksimulation.cpp -I ~/Ox/firmware/lib/header -lm
	

pidcontroller.o: pidcontroller.cpp
	g++ -c pidcontroller.cpp  -I ~/Ox/firmware/lib/header -lm

main: main.o pidcontroller.o mocksimulation.o
	g++ -o main.exe main.o pidcontroller.o mocksimulation.o -lm
