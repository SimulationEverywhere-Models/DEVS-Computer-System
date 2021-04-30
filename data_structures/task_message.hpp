#ifndef TASK_MESSAGE_HPP
#define TASK_MESSAGE_HPP

#include <assert.h>
#include <iostream>
#include <string>

using namespace std;

/*******************************************/
/**************** TaskMessage_t ****************/
/*******************************************/
struct TaskMessage_t{
  TaskMessage_t(){}
  TaskMessage_t(int i_id, int i_workUnits) : id(i_id), work_units(i_workUnits){}
  	int   id;
  	int   work_units;
};

istream& operator>> (istream& is, TaskMessage_t& msg);

ostream& operator<<(ostream& os, const TaskMessage_t& msg);


#endif // TASK_MESSAGE_HPP
