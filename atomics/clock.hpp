/**
* Glenn Davidson
* Clock:
* Cadmium implementation of a digital clock
*/

#ifndef _CLOCK_HPP__
#define _CLOCK_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>

using namespace cadmium;
using namespace std;

/***** (1) *****/
//Port definition
struct Clock_defs{
    struct out : public out_port<bool> {};
    struct in : public in_port<bool> {};
};

/***** (2) *****/
template<typename TIME> class Clock{

    public:
    // ports definition
    using input_ports=tuple<typename Clock_defs::in>;
    using output_ports=tuple<typename Clock_defs::out>;

    /***** (3) *****/
    // state definition
    struct state_type{
        bool clk_enabled;
        bool clk_high;
    };
    state_type state;

    /***** (4) *****/
    // default constructor
    Clock() {
        state.clk_enabled = false;
        state.clk_high = false;
    }

    /***** (5) *****/
    // internal transition
    void internal_transition() {
        if(state.clk_high == true){
          state.clk_high = false;
        }
        else{
          state.clk_high = true;
        }
    }

    /***** (6) *****/
    // external transition
    // change this logic to : enable if the inpute just changed to enable, disable if the input just changed to disable
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        // was a vector of Message_t
        vector<bool> bag_port_in;
        bag_port_in = get_messages<typename Clock_defs::in>(mbs);
        if(bag_port_in.size()>1) assert(false && "One message at a time");
        state.clk_enabled = bag_port_in[0];
        
        if(state.clk_enabled == true){
          state.clk_high = true;
        }
        else{
          state.clk_high = false;
        }
    }

    /***** (7) *****/
    // confluence transition
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), move(mbs));
    }

    /***** (8) *****/
    // output function
    typename make_message_bags<output_ports>::type output() const {
        // want to output the bool clk_high
        typename make_message_bags<output_ports>::type bags;
        vector<bool> bag_port_out;
        bag_port_out.push_back(state.clk_high);
        get_messages<typename Clock_defs::out>(bags) = bag_port_out;
        return bags;
    }

    /***** (9) *****/
    // time_advance function
    TIME time_advance() const {
        TIME next_internal;
        if (state.clk_enabled == true) {
            next_internal = TIME("00:00:00:500");
        }else {
            next_internal = numeric_limits<TIME>::infinity();
        }
        return next_internal;
    }

    /***** (10) *****/
    friend ostringstream& operator<<(ostringstream& os, const typename Clock<TIME>::state_type& i) {
        os << "clk_high: " << i.clk_high << " & clk_enabled: " << i.clk_enabled;
        return os;
    }
};
#endif // _Clock_HPP__
