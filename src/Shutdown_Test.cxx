#include <linux/reboot.h>
#include <sys/reboot.h>
#include <unistd.h>
#include <iostream>

int main()
{
  std::cerr << "About to shutdown.\n";
  std::cerr << "Sync\n";
  sync();

  std::cerr << "Shutdown\n";
  reboot(LINUX_REBOOT_CMD_POWER_OFF);
  std::cerr << "called shutdown\n";
}

