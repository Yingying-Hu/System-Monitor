#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;
using namespace LinuxParser;

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return cpu; }

// TODO: Return the command that generated this process
string Process::Command() { 
  if (cmdline.length()>40){
    cmdline = cmdline.substr(0,40).append("...");
    }
  return cmdline; 
  }

// TODO: Return this process's memory utilization
string Process::Ram() { return ram; }

// TODO: Return the user (name) that generated this process
string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return uptime; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { return this->cpu < a.cpu; }

bool Process::operator>(Process const& a) const { return this->cpu > a.cpu; }

void Process::Pid(int pid) { pid_ = pid; }

void Process::User(int pid) { user_ = LinuxParser:: User(pid); }

void Process::Command(int pid) { cmdline = LinuxParser::Command(pid); }

void Process::CpuUtilization(int pid) {
  float total_time_sec = LinuxParser::ActiveJiffies(pid) / sysconf(_SC_CLK_TCK);
  cpu = total_time_sec / (float)LinuxParser::UpTime(pid);
}

void Process::Ram(int pid) {
  ram = LinuxParser::Ram(pid);
}

void Process::UpTime(int pid) { uptime = LinuxParser::UpTime(pid); }