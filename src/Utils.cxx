#include "Utils.hxx"
#include <chrono>
#include <ctime>

std::string BlackBox::utcStamp()
{
  // get the current time string in MMDDYY_HHMMSS
  using std::chrono::system_clock;
  auto tt_now = system_clock::to_time_t(system_clock::now());

  char buffer[100];
  strftime(buffer, 100, "%Y%m%d_%H%M%S", gmtime(&tt_now));
  return std::string(buffer);
}

std::string BlackBox::timeToFilename(const std::string & prefix, const std::string & suffix)
{
  
  // cat the strings together
  return prefix + utcStamp() + suffix;
}
