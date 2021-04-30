/**
* Gkenn Davidson
*
* FifoQueue:
* Cadmium implementation of FifoQueue atomic model using c++ stl queue structure
*/

#ifndef _FIFO_QUEUE_HPP__
#define _FIFO_QUEUE_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>
#include <queue>

#include "../data_structures/task_message.hpp"
#define MAX_QUEUE_SIZE  4
#define WAITING         "WAITING"
#define REQUESTED       "REQUESTED"
#define POPTASK         "POPTASK"
#define PUSHTASK        "PUSHTASK"

using namespace cadmium;
using namespace std;

/***** (1) *****/
//Port definition
struct FifoQueue_defs{
    struct task_out : public out_port<TaskMessage_t> {};
    struct size_out : public out_port<int> {};
    struct task_in : public in_port<TaskMessage_t> {};
    struct givetask_in : public in_port<bool> {};
};

/***** (2) *****/
template<typename TIME> class FifoQueue{

    public:
    // ports definition
    using input_ports=tuple<typename FifoQueue_defs::task_in, typename FifoQueue_defs::givetask_in>;
    using output_ports=tuple<typename FifoQueue_defs::task_out, typename FifoQueue_defs::size_out>;

    /***** (3) *****/
    // state definition
    struct state_type{
          // state variables are queue structure, and QueueState
          string QueueState;
          queue<TaskMessage_t> TaskQueue;
          TaskMessage_t OutputTask;
    };
    state_type state;

    /***** (4) *****/
    // default constructor
    FifoQueue() {
        state.QueueState = WAITING;
        // default blank queue -- assuming the default constrcutor makes a size of queue
    }

    /***** (5) *****/
    // internal transition
    void internal_transition() {
        if(state.QueueState == PUSHTASK){
          state.QueueState = WAITING;
        }
        else if(state.QueueState == POPTASK){
          state.QueueState = WAITING;
        }

        else{
          assert(false && "ERROR IN INTERAL TRANSITION FUNCITON");
        }
    }

    /***** (6) *****/
    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {

        // fetch two inputs from the input message bag
        vector<TaskMessage_t> bag_port_in_msg;
        vector<bool> bag_port_in_bool;
        bag_port_in_msg = get_messages<typename FifoQueue_defs::task_in>(mbs);
        bag_port_in_bool = get_messages<typename FifoQueue_defs::givetask_in>(mbs);

        // ensure that at most there is 1 input of each type
        if(bag_port_in_msg.size()>1) assert(false && "One msg at a time");
        if(bag_port_in_bool.size()>1) assert(false && "One givetask at a time");
        if(bag_port_in_bool.size() == 1 && bag_port_in_msg.size() == 1) assert(false && "One input at a time");

        // a new task arrives to the queue
        if(bag_port_in_msg.size() == 1){

          if((state.QueueState == WAITING) && (state.TaskQueue.size() < MAX_QUEUE_SIZE)){
            // if the state is WAITING and the queue is not full, add task to queue
            state.TaskQueue.push(bag_port_in_msg[0]);
            state.QueueState = PUSHTASK;
          }
          // otherwise ignore the newly arrived task
        }

        // a request has been made to the queue to output a task
        else if(bag_port_in_bool.size() == 1){

          // if there are items in the queue, the state is waiting, and givetask is low (processor is not busy)
          bool givetask = !bag_port_in_bool[0]; // inverted because the processor outputs true when busy
          bool should_give_task = ((givetask == true) && (state.QueueState == WAITING) && (state.TaskQueue.size() > 0));
          if(should_give_task){
            state.OutputTask =  state.TaskQueue.front();
            state.TaskQueue.pop();
            state.QueueState = POPTASK;
          }
          else{

          }
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
        typename make_message_bags<output_ports>::type bags;

        vector<int> bag_port_out_int;

        if(state.QueueState == PUSHTASK){
            // output the current size of the TaskQueue
            bag_port_out_int.push_back(state.TaskQueue.size());
            get_messages<typename FifoQueue_defs::size_out>(bags) = bag_port_out_int;
        }
        else if(state.QueueState == POPTASK){
          vector<TaskMessage_t> bag_port_out_task;
          bag_port_out_task.push_back(state.OutputTask);
          get_messages<typename FifoQueue_defs::task_out>(bags) = bag_port_out_task;

          bag_port_out_int.push_back(state.TaskQueue.size());
          get_messages<typename FifoQueue_defs::size_out>(bags) = bag_port_out_int;
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
        if (state.QueueState == POPTASK) {
            next_internal = TIME();
        }
        else if (state.QueueState == PUSHTASK) {
            next_internal = TIME();
        }
        else if (state.QueueState == WAITING){
            next_internal = numeric_limits<TIME>::infinity();
        }
        else if (state.QueueState == REQUESTED){
            next_internal = numeric_limits<TIME>::infinity();
        }
        else{
          assert(false && "ERROR IN TIME ADVANCE FUNCTION");
        }
        return next_internal;
    }

    /***** (10) *****/
    friend ostringstream& operator<<(ostringstream& os, const typename FifoQueue<TIME>::state_type& i) {
        os << "state: " << i.QueueState << " & queue size: " << i.TaskQueue.size() << endl;
        queue<TaskMessage_t> q = i.TaskQueue;
        os << "QUEUE DATA:";
        while(!q.empty()){
          os << '\t' << q.front();
          q.pop();
        }
        return os;
    }
};
#endif // _PROCESSOR_HPP__
