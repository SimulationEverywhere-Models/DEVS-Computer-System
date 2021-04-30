/**
* Glenn Davidson
*
* Processor:
* Cadmium implementation of Processor atomic model
*/

#ifndef _PROCESSOR_HPP__
#define _PROCESSOR_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>

#include "../data_structures/task_message.hpp"
#define IDLE  "idle"
#define BEGIN "begin"
#define BUSY  "busy"
#define DONE  "done"

using namespace cadmium;
using namespace std;

/***** (1) *****/
//Port definition
struct Processor_defs{
    struct task_out : public out_port<TaskMessage_t> {};
    struct busy_out : public out_port<bool> {};
    struct clk_in : public in_port<bool> {};
    struct task_in : public in_port<TaskMessage_t> {};
};

/***** (2) *****/
template<typename TIME> class Processor{

    public:
    // ports definition
    using input_ports=tuple<typename Processor_defs::clk_in, typename Processor_defs::task_in>;
    using output_ports=tuple<typename Processor_defs::task_out, typename Processor_defs::busy_out>;

    /***** (3) *****/
    // state definition
    struct state_type{
          // state variables are queue structure, and InternalState
          string InternalState;
          TaskMessage_t CurrentTask;
          int IdleCount;
    };
    state_type state;

    /***** (4) *****/
    // default constructor
    Processor() {
        state.InternalState = IDLE;
        state.IdleCount = 0;
        // default initialization for TaskMessage_t
    }

    /***** (5) *****/
    // internal transition
    void internal_transition() {
        if(state.InternalState == BEGIN){
          state.InternalState = BUSY;
        }
        else if(state.InternalState == BUSY){
          state.InternalState = DONE;
        }
        else if(state.InternalState == DONE){
          state.InternalState = IDLE;
          state.CurrentTask.id = 0;
          state.CurrentTask.work_units = 0;
          state.IdleCount = 0;
        }
        else{
          assert(false && "ERROR IN INTERAL TRANSITION FUNCITON");
        }
    }

    /***** (6) *****/
    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {

        // fetch two inputs from the input message bag - possibilities are Clk and NewTask
        vector<TaskMessage_t> bag_port_in_task;
        vector<bool> bag_port_in_bool;
        bag_port_in_task = get_messages<typename Processor_defs::task_in>(mbs);
        bag_port_in_bool = get_messages<typename Processor_defs::clk_in>(mbs);

        // ensure that at most there is 1 input of each type
        if(bag_port_in_task.size()>1) assert(false && "One msg at a time");
        if(bag_port_in_bool.size()>1) assert(false && "One givetask at a time");
        //if(bag_port_in_bool.size() == 1 && bag_port_in_task.size() == 1) assert(false && "Task arrived at rising clk edge");

        // case the processor detects a rising clock edge
        if(bag_port_in_bool.size() == 1){
          bool isRisingEdge = bag_port_in_bool[0];
          // if the procesor is busy and a rising edge is detected, perform one work unit on the task
          if(state.InternalState == BUSY && isRisingEdge){
            state.CurrentTask.work_units--;
            if(state.CurrentTask.work_units <= 0){
              state.InternalState = DONE;
            }
          }
          // if the processor is idle, we detect a clock rising edge, and no task has arrived with the clock edge, increase idle count
          else if(state.InternalState == IDLE && isRisingEdge && bag_port_in_task.size() == 0 ){
            state.IdleCount++;
            if(state.IdleCount == 1){
              state.InternalState = DONE;
            }
          }
        }
        // case a new task arrives at the processor input
        if(bag_port_in_task.size() == 1 && state.InternalState == IDLE){

          // if the processor is idle, accept the task and begin processing
          state.CurrentTask = bag_port_in_task[0];
          if(state.CurrentTask.work_units < 0) assert(false && "Invalid Task submitted to the processor");
          state.InternalState = BEGIN;
        }

        // otherwise ignore arriving tasks and clock edges
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
        typename make_message_bags<output_ports>::type bags;

        vector<bool> bag_port_out_bool;

        if(state.InternalState == BEGIN){
            // output that the processer is now busy
            bag_port_out_bool.push_back(true);
            get_messages<typename Processor_defs::busy_out>(bags) = bag_port_out_bool;
        }
        else if(state.InternalState == DONE){

          // output that the processer is now idle
          bag_port_out_bool.push_back(false);
          get_messages<typename Processor_defs::busy_out>(bags) = bag_port_out_bool;

          vector<TaskMessage_t> bag_port_out_task;
          bag_port_out_task.push_back(state.CurrentTask);
          get_messages<typename Processor_defs::task_out>(bags) = bag_port_out_task;

        }
        else{
          assert(false && "ERROR IN OUTPUT FUNCITON");
        }

        return bags;
    }

    /***** (9) *****/
    // time_advance function
    TIME time_advance() const {
        TIME next_internal;
        if (state.InternalState == IDLE) {
            next_internal = numeric_limits<TIME>::infinity();
        }
        else if (state.InternalState == BEGIN) {
            next_internal = TIME();
        }
        else if (state.InternalState == BUSY){
            next_internal = numeric_limits<TIME>::infinity();
        }
        else if (state.InternalState == DONE){
            next_internal = TIME();
        }
        else{
          assert(false && "ERROR IN TIME ADVANCE FUNCTION");
        }
        return next_internal;
    }

    /***** (10) *****/
    friend ostringstream& operator<<(ostringstream& os, const typename Processor<TIME>::state_type& i) {
        os << "state: " << i.InternalState << " Current Task: " << i.CurrentTask << " Idle Count: " << i.IdleCount;
        return os;
    }
};
#endif // _PROCESSOR_HPP__
