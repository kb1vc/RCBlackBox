#pragma once
#include <string>

namespace BlackBox {
  class Video {
  public:
    Video();

    bool openVidFile(const std::string & fname);

    void start();

    void stop();

    void pause();
    
  protected:
    std::string video_fname;
    int pid; 
  };
}
