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
  };
}
