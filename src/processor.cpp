#include "processor.h"
#include "linux_parser.h"
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  float active = LinuxParser::ActiveJiffies();
  float idle = LinuxParser::IdleJiffies();
  if (active + idle > 0.0f){
    return active / (active + idle);
  }
  else {return 0.0f;}
}