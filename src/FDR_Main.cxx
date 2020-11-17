#include "FXAS21002C.hxx"
#include "FXOS8700CQ.hxx"
#include <iostream>
#include <unistd.h>
#include <string>
#include "FDR.hxx"
#include <linux/reboot.h>
#include <sys/reboot.h>
#include <boost/program_options.hpp>


namespace po = boost::program_options;

class Options {
public:
  Options(int argc, char ** argv) : desc("Options") {
    makeOptions();

    boost::program_options::variables_map pmap;
    
    po::store(po::parse_command_line(argc, argv, desc), pmap);
    po::notify(pmap);

    if(pmap.count("help")) {
      std::cout << desc << std::endl;
      exit(-1);
    }
  }

  bool shutdownOnEnd() { return shutdown_on_end; }
  bool enableCamera() { return !no_camera; }
  bool enableSensors() { return !no_sensors; }
  bool usePIOServer() { return enable_pio_server; }

  void makeOptions() {
    desc.add_options()
      ("help", "help message")
      ("shutdown", po::value<bool>(&shutdown_on_end)->default_value(false)->implicit_value(true), "Shutdown pi when recording is done")
      ("use_pio_server", po::value<bool>(&enable_pio_server)->default_value(false)->implicit_value(true), "Use PIO server")
      ("no_sensors", po::value<bool>(&no_sensors)->default_value(false)->implicit_value(true), "Disable gyro, compass, accel sensors")
      ("no_camera", po::value<bool>(&no_camera)->default_value(false)->implicit_value(true), "Disable camera");
  }
  
  po::options_description desc; // std::string("Options"));

  bool shutdown_on_end;
  bool no_sensors, no_camera;
  bool enable_pio_server; 
};

int fiddleNetwork(bool on) {
  const char * state = on ? "up" : "down";

  int pid = fork();
  if(pid == 0) {
    int stat = execl("/sbin/ip", "/sbin/ip", "link", "set", "wlan0", state, NULL);

    std::cerr << "execl got " << stat << " errno = " << errno << "\n";
  }
  
  return 0;
}

int main(int argc, char * argv[]) {
  // create a connection. We're going to use
  // GPIO 4 and 17 for the interrupts. (4 for the Gyro FIFO interrupt, 17)
  BlackBox::PIIO * piio_p; 

  Options options(argc, argv);

  bool use_piio_server = ((argc > 1) && (argv[1][0] == 'd'));

  // Some RC receivers have trouble when the pi
  // starts yelling to find a wireless network.
  // we need to shut the wireless network off.
  fiddleNetwork(false);
  
  // open the fdr object.
  BlackBox::FDR fdr(options.usePIOServer(), options.enableCamera(), options.enableSensors());
  
  // start the devices
  fdr.openLog("FDR_");
  
  fdr.run();

  // once we get here, it is time to shutdown (only if ARGV[1] is "SHUTDOWN" (note caps).
  if(options.shutdownOnEnd()) {
    std::cerr << "Shutting down.\n";
    std::cout.flush();
    std::cerr.flush();
    sync();
    reboot(LINUX_REBOOT_CMD_POWER_OFF);
  }
  else {
    fiddleNetwork(true);  
  }
}
