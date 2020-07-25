#include "Video.hxx"
#include <iostream>

namespace BlackBox {
  Video::Video() {
  }

  bool Video::openVidFile(const std::string & fname) {
    std::cerr << fname << "\n";
    return true; 
  }

  void Video::start() {
  }

  void Video::stop() {
  }

  void Video::pause() {
  }
  
}
