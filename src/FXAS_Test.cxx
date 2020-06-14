#include "FXAS21002C.hxx"
#include <iostream>
#include <unistd.h>
#include "PIIOD.hxx"
#include "PIIORaw.hxx"

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


int main(int argc, char * argv[]) {
  // create a connection. We're going to use
  // GPIO 4 and 17 for the interrupts. (4 for the FIFO interrupt.)
  BlackBox::PIIO * piio_p; 
  
  if((argc > 1) && (argv[1][0] == 'd')) {
    piio_p = new BlackBox::PIIOD;
  }
  else {
    piio_p = new BlackBox::PIIORaw; 
  }

  BlackBox::FXAS21002C gyro(piio_p, 1, 0x21, 4, BlackBox::FXAS21002C::FIFO);

  //  gyro.start(BlackBox::FXAS21002C::CR1_DATA_RATE_25);
  gyro.start(BlackBox::FXAS21002C::CR1_DATA_RATE_200);

  BlackBox::Rates rates[256];

  int xa, ya, za;
  xa = ya = za = 0;
  
  int xcorr, ycorr, zcorr;
  xcorr = ycorr = zcorr = 0;
  // ignore the first second or so as the device settles.
  int i; 
  for(i = 0; i < 10; ) { //1000; ) {
    int numrates = gyro.getRates(256, rates);
    i += numrates;
  }
  // Measure the drift rate as the device is still
  for(i = 0; i < 100; ) { // 1000; ) {
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
  
  
  std::cerr << "### Corrections: " << i << " " << xcorr / 1000 << " " << ycorr / 1000 << " " << zcorr / 1000 << "\n";
  for(int i = 0; i < 5000; ) {
    int numrates = gyro.getRates(256, rates);
    if(numrates > 0) {
      xa = ya = za = 0;
      for(int j = 0; j < numrates; j++) {
	int x, y, z;
	x = swapEnds(rates[j].x);
	y = swapEnds(rates[j].y);
	z = swapEnds(rates[j].z);
	xa += x;
	ya += y;
	za += z;
      }

    
      int inc = numrates / 2; 
      xa = (xa + inc) / numrates;
      ya = (ya + inc) / numrates;
      za = (za + inc) / numrates;
    
      std::cout << std::dec << (i += numrates) << " "
		<< numrates << " "
		<< rates[0].seq_no << " "
		<< xa << " " << ya << " " << za << "\n";
      std::cout.flush();
    }
    usleep(100);
  }
}
