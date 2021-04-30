//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>

//Messages structures
#include "../data_structures/task_message.hpp"

//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/processor.hpp"
#include "../atomics/clock.hpp"
#include "../atomics/fifo_queue.hpp"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/
// using top_task_in
struct in_enable: public in_port<bool>{};

/***** Define output port for coupled models *****/
// using top_task_out
struct out_busy: public out_port<bool>{};

/***** Define input port for top models *****/
// none

/***** Define output ports for top model *****/
struct top_task_out: public out_port<TaskMessage_t>{};
struct top_task_in: public in_port<TaskMessage_t>{};

/****** Input Readers******/
//        - one for new tasks arriving : TaskMessage_t
//        - one for clk enable : bool

template<typename T>
class InputReader_TaskMessage : public iestream_input<TaskMessage_t,T> {
    public:
        InputReader_TaskMessage () = default;
        InputReader_TaskMessage (const char* file_path) : iestream_input<TaskMessage_t,T>(file_path) {}
};

template<typename T>
class InputReader_bool : public iestream_input<bool,T> {
    public:
        InputReader_bool () = default;
        InputReader_bool (const char* file_path) : iestream_input<bool,T>(file_path) {}
};

int main(){

    /****** Input Readers atomic model instantiation *******************/
    // input reader new task inputs to the processor model
    const char * i_input_data1 = "../input_data/coupled_processor_input.txt";
    shared_ptr<dynamic::modeling::model> input_reader_newtask;
    input_reader_newtask = dynamic::translate::make_dynamic_atomic_model<InputReader_TaskMessage, TIME, const char*>("input_reader_newtask", move(i_input_data1));

    // input reader for hardware clk enable
    const char * i_input_data2 = "../input_data/hardware_enable.txt";
    shared_ptr<dynamic::modeling::model> input_reader_enable;
    input_reader_enable = dynamic::translate::make_dynamic_atomic_model<InputReader_bool, TIME, const char*>("input_reader_enable", move(i_input_data2));

    /****** Processor atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> processor1;
    processor1 = dynamic::translate::make_dynamic_atomic_model<Processor, TIME>("processor1");

    /****** Clock atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> clock1;
    clock1 = dynamic::translate::make_dynamic_atomic_model<Clock, TIME>("clock1");

    /****** Queue atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> fifo_queue1;
    fifo_queue1 = dynamic::translate::make_dynamic_atomic_model<FifoQueue, TIME>("fifo_queue1");

    /*******PROCESSOR-CLK COUPLED MODEL (HARDWARE)********/
    dynamic::modeling::Ports iports_HW = {typeid(top_task_in), typeid(in_enable)};
    dynamic::modeling::Ports oports_HW = {typeid(top_task_out), typeid(out_busy)};
    dynamic::modeling::Models submodels_HW = {processor1, clock1};
    dynamic::modeling::EICs eics_HW = {
      dynamic::translate::make_EIC<top_task_in,Processor_defs::task_in>("processor1"),
      dynamic::translate::make_EIC<in_enable,Clock_defs::in>("clock1")
    };
    dynamic::modeling::EOCs eocs_HW = {
      dynamic::translate::make_EOC<Processor_defs::task_out,top_task_out>("processor1"),
      dynamic::translate::make_EOC<Processor_defs::busy_out,out_busy>("processor1")
    };
    dynamic::modeling::ICs ics_HW = {
      dynamic::translate::make_IC<Clock_defs::out, Processor_defs::clk_in>("clock1","processor1")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> HARDWARE;
    HARDWARE = make_shared<dynamic::modeling::coupled<TIME>>(
        "HARDWARE", submodels_HW, iports_HW, oports_HW, eics_HW, eocs_HW, ics_HW
    );

    /*******Hardware-Queue CSYS Coupled Model********/
    dynamic::modeling::Ports iports_CSYS = {typeid(top_task_in), typeid(in_enable)};
    dynamic::modeling::Ports oports_CSYS = {typeid(top_task_out)};
    dynamic::modeling::Models submodels_CSYS = {fifo_queue1, HARDWARE};
    dynamic::modeling::EICs eics_CSYS = {
      dynamic::translate::make_EIC<top_task_in,FifoQueue_defs::task_in>("fifo_queue1"),
      dynamic::translate::make_EIC<in_enable,in_enable>("HARDWARE")
    };
    dynamic::modeling::EOCs eocs_CSYS = {
      dynamic::translate::make_EOC<top_task_out,top_task_out>("HARDWARE")
    };
    dynamic::modeling::ICs ics_CSYS = {
      dynamic::translate::make_IC<FifoQueue_defs::task_out, top_task_in>("fifo_queue1","HARDWARE"),
      dynamic::translate::make_IC<out_busy, FifoQueue_defs::givetask_in>("HARDWARE","fifo_queue1")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> COMP_SYS;
    COMP_SYS = make_shared<dynamic::modeling::coupled<TIME>>(
        "COMP_SYS", submodels_CSYS, iports_CSYS, oports_CSYS, eics_CSYS, eocs_CSYS, ics_CSYS
    );

    /*******TOP MODEL - COMP_SYS with two input readers********/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(top_task_out)};
    dynamic::modeling::Models submodels_TOP = {input_reader_newtask, input_reader_enable, COMP_SYS};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC<top_task_out,top_task_out>("COMP_SYS")
    };
    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<TaskMessage_t>::out,top_task_in>("input_reader_newtask","COMP_SYS"),
        dynamic::translate::make_IC<iestream_input_defs<bool>::out,in_enable>("input_reader_enable","COMP_SYS")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/main_test_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/main_test_output_state.txt");
    struct oss_sink_state{
        static ostream& sink(){
            return out_state;
        }
    };

    using state=logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;
    using log_messages=logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_mes=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_sta=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using logger_top=logger::multilogger<state, log_messages, global_time_mes, global_time_sta>;

    /***** (7) *****/
    /************** Runner call ************************/
    dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    r.run_until(NDTime("04:00:00:000"));
    return 0;
}
