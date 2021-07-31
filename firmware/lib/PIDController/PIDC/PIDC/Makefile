main.o: main.cpp
	gcc -c main.cpp -I MockSim -l ~/PubInv/Ox/firmware/lib

MockSim/mocksimulation.o: MockSim/mocksimulation.cpp
	cd MockSim; gcc -c mocksimulation.cpp -l ~/PubInv/Ox/firmware/lib

pidcontroller.o: pidcontroller.cpp
	gcc -c pidcontroller.cpp  -l ~/PubInv/Ox/firmware/lib

main: main.o pidcontroller.o MockSim/mocksimulation.o
	gcc -o main.exe make.o pidcontroller.o MockSim/mocksimulation.o
