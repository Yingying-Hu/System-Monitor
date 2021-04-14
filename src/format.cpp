#include <sstream>
#include <string>

#include "format.h"

using std::string;

#define HOUR 3600
#define MIN 60

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int h = seconds / HOUR;
  int sec = seconds % HOUR;
  int min = sec / MIN;
  sec %= MIN;

  std::ostringstream out_stream;
  out_stream.width(2);
  out_stream.fill('0');
  out_stream << h << ":";
  out_stream.width(2);
  out_stream.fill('0');
  out_stream << min << ":";
  out_stream.width(2);
  out_stream.fill('0');
  out_stream << sec;

  return out_stream.str();
}
// sudo apt install clang-format