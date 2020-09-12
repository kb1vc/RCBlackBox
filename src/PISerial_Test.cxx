#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

int main() {
  int stat = gpioInitialise();
  
  if(stat == PI_INIT_FAILED) {
    fprintf(stderr, "Initialization failed.\n");
    exit(-1);
  }
  else {
    fprintf(stderr, "Init returned version 0x%x %d\n", stat, stat);
  }

  stat = gpioSerialReadOpen(23, 115200, 8);

  std::cerr << "serial read open got " << stat << "\n";
  
  stat = gpioSerialReadInvert(23, PI_BB_SER_INVERT);

  std::cerr << "serial read invert got " << stat << "\n";

  char buf[128];
  
  while(1) {
    stat = gpioSerialRead(23, buf, 128);

    if(stat < 0) {
      std::cerr << "Ooops -- bad stat " << stat << "\n";
    }
    else if(stat > 0) {
      for(int i = 0; i < stat; i += 16) {
	printf("%2d : ", i);
	
	for(int j = 0; (j < 16) && ((i + j) < stat); j++) {
	  printf(" %02x", buf[i+j]);
	}
	printf("\n");
      }
    }
  }
}
