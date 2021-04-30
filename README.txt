This folder contains the ComputerSystem DEVS model implemented in Cadmium. The list of files contained and the instructions to run the tests and top model are as follows:

/**************************/
/****Organization of Files****/
/**************************/

README.txt	
ComputerSystem Assignment Report.docx
makefile
ModelDescription-ComputerSystem.xml

atomics [This folder contains atomic models implemented in Cadmium]
	clock.hpp
	fifo_queue.hpp
	processor.hpp
bin [This folder will be created automatically when compiling target tests with the makefile, and will contain the text executables]
build [This folder will be created automatically when compiling target tests with the makefile, It will contain all the build files (.o) generated during compilation]
data_structures [This folder contains a TaskMessage data structure used in the queue model]
	task_message.hpp
	task_message.cpp
input_data [This folder contains all the input data to run the atomic tests and main top model test]

	FOR ATOMIC TESTS:
	clock_input_test.txt
	queue_givetask_input_test.txt
	queue_newtask_input_test.txt
	processor_clk_input_test.txt
	processor_task_input_test.txt
	
	FOR TOP MODEL TESTS:
	hardware_enable.txt
	coupled_processor_input.txt
	
simulation_results [This folder will be created automatically the first time you compile the poject. It will store the simulation logs of the tests]
test [This folder the unit test of the atomic models]
	main_clock_test.cpp
	main_queue_test.cpp
	main_processor_test.cpp
top_model [the folder containing the top model .cpp]
vendor [folder containing structures essential for model]

/*************/
/****STEPS****/
/*************/

0 - ComputerSystem Assignment Report.docx and ModelDescription-ComputerSystem.xml describe the project and tests

1 - Using an a Ubuntu install of CADMIUM: extract ComputerSystem.zip to the folder HOME/CADMIUM/Cadmium-Simulation-Environment/DEVS-Models
	If another operating system or install directory is desired, change the makefile accordingly

2 - Compile the project and the tests by:
	1 - Opening a terminal and navigating to the main project directory containing the makefile
	2 - To compile the atomic tests, type in the terminal:
			make clean; make tests
	3 - To compile the top model and the atomic tests at the same time
			make clean; make all
3 - Run CLOCK_TEST, QUEUE_TEST, and PROCESSOR_TEST to verify atomic model:
	1 - Open the terminal in the bin folder. 
	2 - To run the test, type in the terminal "./NAME_OF_THE_COMPILED_FILE" 
	3 - To check the output of the test, go to the folder simulation_results and open the associated text files
	
4 - Run the top model OS:
	1 - Open the terminal in the bin folder. 
	2 - To run the test, type in the terminal "./OS" 
	3 - To check the output of the test, go to the folder simulation_results and open main_test_output_state.txt and main_test_output_messages.txt
