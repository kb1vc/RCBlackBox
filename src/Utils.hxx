#pragma once
#include <string>

namespace BlackBox {

  std::string utcStamp();
  
  std::string timeToFilename(const std::string & prefix, const std::string & suffix);

}

