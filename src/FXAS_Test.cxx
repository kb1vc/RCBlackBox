#include "FXAS21002C.hxx"
#include <iostream>
#include <unistd.h>

short swapEnds(unsigned short v)  {
  short ret;

  ret = ((v & 0xff) << 8) | ((v >> 8) & 0xff);

  return ret; 
}
int main() {
  // create a connection. We're going to use
  // GPIO 4 and 17 for the interrupts. (4 for the FIFO interrupt.)

  BlackBox::FXAS21002C gyro(1, 0x21, 4, BlackBox::FXAS21002C::FIFO);

  gyro.start(BlackBox::FXAS21002C::CR1_DATA_RATE_25);

  BlackBox::Rates rates[256];

  int xa, ya, za;
  xa = ya = za = 0;
  for(int i = 0; i < 10000; ) {
    int numrates = gyro.getRates(256, rates);
    for(int j = 0; j < numrates; j++) {
      std::cerr << std::dec << i++ << " "
		<< numrates << " "
		<< rates[j].seq_no << " "
		<< swapEnds(rates[j].x) << " " 
		<< swapEnds(rates[j].y) << " "
		<< swapEnds(rates[j].z) << "\n";
    }
    usleep(10000);
  }
}
