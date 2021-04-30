This folder contains the ComputerSystem DEVS model implemented in Cadmium

/**************************/
/****Organization of Files****/
/**************************/

README.txt	
computersystemr1.docx
makefile

atomics [This folder contains atomic models implemented in Cadmium]
	clock.hpp
	fifo_queue.hpp
bin [This folder will be created automatically when compiling target tests with the makefile, and will contain the text executables]
build [This folder will be created automatically when compiling target tests with the makefile, It will contain all the build files (.o) generated during compilation]
data_structures [This folder contains a TaskMessage data structure used in the queue model]
	task_message.hpp
	task_message.cpp
input_data [This folder contains all the input data to run the model and the tests]
	clock_input_test.txt
	queue_givetask_input_test.txt
	queue_newtask_input_test.txt
simulation_results [This folder will be created automatically the first time you compile the poject. It will store the simulation logs of the tests]
test [This folder the unit test of the atomic models]
	main_clock_test.cpp
	main_queue_test.cpp

/*************/
/****STEPS****/
/*************/

0 - computersystemr1.docx descirbes the state of this project

1 - using an a Ubuntu install of CADMIUM: extract ComputerSystem_r1.zip to the folder HOME/CADMIUM/Cadmium-Simulation-Environment/DEVS-Models
2 - Compile the project and the tests
	1 - Open a terminal in the main project directory
	3 - To compile the atomic tests, type in the terminal:
			make clean; make tests
3 - Run CLOCK_TEST and QUEUE_TEST
	1 - Open the terminal in the bin folder. 
	2 - To run the test, type in the terminal "./NAME_OF_THE_COMPILED_FILE" 
	3 - To check the output of the test, go to the folder simulation_results and open the associated text files
