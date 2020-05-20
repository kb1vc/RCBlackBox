#include "FXAS21002C.hxx"
#include <iostream>
#include <unistd.h>

short swapEnds(unsigned short v)  {
  short ret;

  ret = ((v & 0xff) << 8) | ((v >> 8) & 0xff);

  return ret; 
}

int divit(int a, int b) {
  // get nearest good divide
  int ret = a / b;
  if((a % b) > (b/2)) ret++;

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
  
  int xcorr, ycorr, zcorr;
  xcorr = ycorr = zcorr;
  // ignore the first second or so as the device settles.
  int i; 
  for(i = 0; i < 1000; ) {
    int numrates = gyro.getRates(256, rates);
    i += numrates;
  }
  // Measure the drift rate as the device is still
  for(i = 0; i < 1000; ) {
    int numrates = gyro.getRates(256, rates);
    i += numrates;
    for(int j = 0; j < numrates; j++) {
      int x, y, z;
      x = swapEnds(rates[j].x);
      y = swapEnds(rates[j].y);
      z = swapEnds(rates[j].z);
      xcorr += x;
      ycorr += y;
      zcorr += z; 
    }
  }
  
  
  std::cerr << "### Corrections: " << i << " " << xcorr << " " << ycorr << " " << zcorr << "\n";
  for(int i = 0; i < 2000; ) {
    int numrates = gyro.getRates(256, rates);
    for(int j = 0; j < numrates; j++) {
      int x, y, z;
      x = swapEnds(rates[j].x);
      y = swapEnds(rates[j].y);
      z = swapEnds(rates[j].z);
      xa += x;
      ya += y;
      za += z;
      std::cout << std::dec << i++ << " "
		<< numrates << " "
		<< rates[j].seq_no << " "
		<< x << " " << y << " " << z << " "
		<< xa << " " << ya << " " << za << "\n";
    }
    usleep(10000);
  }
}
