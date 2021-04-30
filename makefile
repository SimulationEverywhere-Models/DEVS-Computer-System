CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I ../../cadmium/include
INCLUDEDESTIMES=-I ../../DESTimes/include

#CREATE BIN AND BUILD FOLDERS TO SAVE THE COMPILED FILES DURING RUNTIME
bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

#TARGET TO COMPILE ALL THE TESTS TOGETHER (NOT SIMULATOR)
task_message.o: data_structures/task_message.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) data_structures/task_message.cpp -o build/task_message.o

main_top.o: top_model/main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) top_model/main.cpp -o build/main_top.o

main_clock_test.o: test/main_clock_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_clock_test.cpp -o build/main_clock_test.o

main_queue_test.o: test/main_queue_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_queue_test.cpp -o build/main_queue_test.o

main_processor_test.o: test/main_processor_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_processor_test.cpp -o build/main_processor_test.o

tests: main_clock_test.o main_queue_test.o main_processor_test.o task_message.o
	$(CC) -g -o bin/CLOCK_TEST build/main_clock_test.o
	$(CC) -g -o bin/QUEUE_TEST build/main_queue_test.o build/task_message.o
	$(CC) -g -o bin/PROCESSOR_TEST build/main_processor_test.o build/task_message.o

#TARGET TO COMPILE ONLY ABP SIMULATOR
simulator: main_top.o task_message.o
	$(CC) -g -o bin/OS build/main_top.o build/task_message.o 

#TARGET TO COMPILE EVERYTHING (ABP SIMULATOR + TESTS TOGETHER)
all: simulator tests

#CLEAN COMMANDS
clean:
	rm -f bin/* build/*
