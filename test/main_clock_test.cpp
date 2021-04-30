//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>

//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/clock.hpp"

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
struct top_out: public out_port<bool>{};

/***** (2) *****/
/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_bool : public iestream_input<bool,T> {
    public:
        // override constructors
        InputReader_bool () = default;
        InputReader_bool (const char* file_path) : iestream_input<bool,T>(file_path) {}
};

/***** (3) *****/
int main(){

    /****** Input Reader atomic model instantiation *******************/
    const char * i_input_data = "../input_data/clock_input_test.txt";
    shared_ptr<dynamic::modeling::model> input_reader;
    input_reader = dynamic::translate::make_dynamic_atomic_model<InputReader_bool, TIME, const char*>("input_reader", move(i_input_data));

    /***** (4) *****/
    /****** Clock atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> clock1;
    clock1 = dynamic::translate::make_dynamic_atomic_model<Clock, TIME>("clock1");

    /***** (5) *****/
    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP;
    iports_TOP = {};
    dynamic::modeling::Ports oports_TOP;
    oports_TOP = {typeid(top_out)};
    dynamic::modeling::Models submodels_TOP;
    submodels_TOP = {input_reader, clock1};
    dynamic::modeling::EICs eics_TOP;
    eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP;
    eocs_TOP = {
        dynamic::translate::make_EOC<Clock_defs::out,top_out>("clock1")
    };
    dynamic::modeling::ICs ics_TOP;
    ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<bool>::out,Clock_defs::in>("input_reader","clock1")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /***** (6) *****/
    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/clock_test_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/clock_test_output_state.txt");
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
