#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"
using std::stof;
using std::string;
using std::to_string;
using std::vector;

//Read data from the filesystem without replacing the line
template <typename T> T LinuxParser::findValueByKey(string const &keyFilter, string const &filename){
  string line, key;
  T value;

  std::ifstream stream(kProcDirectory + filename);
  if(stream.is_open()) {
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> value){
        if (key == keyFilter) {
          return value;
        }
      }
    }
  }
  stream.close();
  return value;
}

//Read the filesystem with a single line
template <typename T> T LinuxParser::getValueOfFile(std::string const &filename) {
  std::string line;
  T value;
  std::ifstream stream(kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  stream.close();
  return value;
}

// Read and return the operating system name
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  filestream.close();
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  stream.close();
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string memTotal = "MemTotal:";
  string memFree = "MemFree:";
  float Total = findValueByKey<float>(memTotal, kMeminfoFilename);// "/proc/memInfo"
  float Free = findValueByKey<float>(memFree, kMeminfoFilename);
  return (Total - Free) / Total;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long up_time;
  string up_time_s;
  string idle_time_s;
  string line;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> up_time_s >> idle_time_s;
  }
  up_time = std::stol(up_time_s);
  stream.close();
  return up_time;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  vector<string> cpu_jiffers = LinuxParser::CpuUtilization();
  long total_jiffers = 0;
  for (auto s : cpu_jiffers) {
    total_jiffers += stol(s);
  }
  return total_jiffers;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  long act_ji;
  string line, skip, utime, stime, cutime, cstime;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 1; i < 14; i++) {
      linestream >> skip;
    }
    linestream >> utime >> stime >> cutime >> cstime;
  }
  act_ji = stol(utime) + stol(stime) + stol(cutime) + stol(cstime);
  filestream.close();
  return act_ji;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return (LinuxParser::Jiffies() - LinuxParser::IdleJiffies());
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_jiffers = LinuxParser::CpuUtilization();
  return stol(cpu_jiffers[3]) + stol(cpu_jiffers[4]);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string cpu;
  vector<string> cpu_time_units;
  string time_unit;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    for (int i = 0; i < 10; i++) {
      linestream >> time_unit;
      cpu_time_units.push_back(time_unit);
    }
  }
  filestream.close();
  return cpu_time_units;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string keyFilter = "processes";
  string filename = kStatFilename;
  int num_proc = findValueByKey<int>(keyFilter, filename);
  return num_proc;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string keyFilter = "procs_running";
  string filename = kStatFilename;
  int num_proc = findValueByKey<int>(keyFilter, filename);
  return num_proc;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  return string(getValueOfFile<string>(to_string(pid) + kCmdlineFilename));
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string keyFilter = "VmData:";
  string filename = to_string(pid) + kStatusFilename;
  string ram = to_string(findValueByKey<long>(keyFilter, filename) / 1000);
  return ram;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string keyFilter = "Uid:";
  string filename = to_string(pid) + kStatusFilename;
  string uid = findValueByKey<string>(keyFilter, filename);
  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  string line, key, user, users, skip, uid_f;

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream stream(line);
      stream >> users >> skip >> uid_f;
      if (uid_f == uid) {
        user = users;
        break;
      }
    }
  }
  filestream.close();
  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::string clock_ticks;
  string line, skip;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 1; i < 22; i++) {
      linestream >> skip;
    }
    linestream >> clock_ticks;
  }
  filestream.close();
  return UpTime()-stol(clock_ticks) / sysconf(_SC_CLK_TCK);
}
