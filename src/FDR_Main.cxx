#include "FXAS21002C.hxx"
#include "FXOS8700CQ.hxx"
#include <iostream>
#include <unistd.h>
#include "FDR.hxx"
#include <linux/reboot.h>
#include <sys/reboot.h>

int main(int argc, char * argv[]) {
  // create a connection. We're going to use
  // GPIO 4 and 17 for the interrupts. (4 for the Gyro FIFO interrupt, 17)
  BlackBox::PIIO * piio_p; 

  bool use_piio_server = ((argc > 1) && (argv[1][0] == 'd'));

  // open the fdr object.
  BlackBox::FDR fdr(use_piio_server);
  
  // start the devices
  fdr.openLog("FDR_");
  
  fdr.run();

  // once we get here, it is time to shutdown (only if ARGV[1] is "SHUTDOWN" (note caps).
  if((argc > 1) && (std::string(argv[1]) == "SHUTDOWN")) {
    std::cerr << "Shutting down.\n";
    sync();
    reboot(LINUX_REBOOT_CMD_POWER_OFF);
  }
}
