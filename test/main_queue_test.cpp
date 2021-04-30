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
#include "../atomics/fifo_queue.hpp"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** (1) *****/
/***** Define input port for coupled models *****/

/***** Define output ports for coupled model *****/
struct top_task_out: public out_port<TaskMessage_t>{};
struct top_size_out: public out_port<int>{};

/***** (2) *****/
/****** Input Reader atomic model declaration *******************/
// will define two input readers:
//        - one for new task arriving : TaskMessage_t
//        - one for new push task request arriving : bool
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

/***** (3) *****/
int main(){

    /****** Input Readers atomic model instantiation *******************/
    // input reader for queue new task input
    const char * i_input_data1 = "../input_data/queue_newtask_input_test.txt";
    shared_ptr<dynamic::modeling::model> input_reader_newtask;
    input_reader_newtask = dynamic::translate::make_dynamic_atomic_model<InputReader_TaskMessage, TIME, const char*>("input_reader_newtask", move(i_input_data1));

    // input reader for queue givetask input
    const char * i_input_data2 = "../input_data/queue_givetask_input_test.txt";
    shared_ptr<dynamic::modeling::model> input_reader_givetask;
    input_reader_givetask = dynamic::translate::make_dynamic_atomic_model<InputReader_bool, TIME, const char*>("input_reader_givetask", move(i_input_data2));

    /***** (4) *****/
    /****** FifoQueue atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> fifo_queue1;
    fifo_queue1 = dynamic::translate::make_dynamic_atomic_model<FifoQueue, TIME>("fifo_queue1");

    /***** (5) *****/
    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP;
    iports_TOP = {};
    dynamic::modeling::Ports oports_TOP;
    oports_TOP = {typeid(top_task_out), typeid(top_size_out)};
    dynamic::modeling::Models submodels_TOP;
    submodels_TOP = {input_reader_newtask, input_reader_givetask, fifo_queue1};
    dynamic::modeling::EICs eics_TOP;
    eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP;
    eocs_TOP = {
        dynamic::translate::make_EOC<FifoQueue_defs::task_out,top_task_out>("fifo_queue1"),
        dynamic::translate::make_EOC<FifoQueue_defs::size_out,top_size_out>("fifo_queue1")
    };
    dynamic::modeling::ICs ics_TOP;
    ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<TaskMessage_t>::out,FifoQueue_defs::task_in>("input_reader_newtask","fifo_queue1"),
        dynamic::translate::make_IC<iestream_input_defs<bool>::out,FifoQueue_defs::givetask_in>("input_reader_givetask","fifo_queue1")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /***** (6) *****/
    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/queue_test_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/queue_test_output_state.txt");
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
