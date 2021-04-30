#include <math.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include "task_message.hpp"

/***************************************************/
/************* Output stream ************************/
/***************************************************/

ostream& operator<<(ostream& os, const TaskMessage_t& msg) {
  os << msg.id << " " << msg.work_units;
  return os;
}

/***************************************************/
/************* Input stream ************************/
/***************************************************/

istream& operator>> (istream& is, TaskMessage_t& msg) {
  is >> msg.id;
  is >> msg.work_units;
  return is;
}
