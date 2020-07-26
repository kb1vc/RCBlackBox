#include "Video.hxx"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

namespace BlackBox {
  Video::Video() {
  }

  bool Video::openVidFile(const std::string & fname) {
    video_fname = fname; 
    std::cerr << "Video: got vid file name [" << video_fname << "]\n";
    return true; 
  }

  void Video::start() {
    std::cerr << "Video::start got vid file name [" << video_fname << "]\n";
    std::string vfar = "--output " + video_fname;
    char * args[6];
    int i = 0;
    char * cmd = (char*) "/usr/bin/raspivid";
    pid = fork();
    if(pid == 0) {
      // execl(cmd, cmd, "-o", video_fname.c_str(), 
      // 		"-t", "0", NULL);
      execl(cmd, cmd, "-o", video_fname.c_str(), 
	    "-t", "0", NULL);
      // // execl("/bin/echo", "/bin/echo", video_fname.c_str(), "bar", NULL);
      // execl("/bin/echo", "/bin/echo", vfar.c_str(), "bar", NULL);      
      
    }
    std::cerr << "pid = " << pid << "\n";    
  }

  void Video::stop() {
    kill(pid, SIGKILL);
  }

  void Video::pause() {
  }
  
}
