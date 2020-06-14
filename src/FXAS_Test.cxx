#include "FXAS21002C.hxx"
#include <iostream>
#include <unistd.h>
#include "PIIOD.hxx"
#include "PIIORaw.hxx"



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

  float xa, ya, za;
  xa = ya = za = 0.0;
  
  int xcorr, ycorr, zcorr;
  xcorr = ycorr = zcorr = 0;
  // ignore the first second or so as the device settles.
  int i; 
  for(i = 0; i < 10; ) { //1000; ) {
    int numrates = gyro.getRates(256, rates);
    i += numrates;
  }
  
  
  float xp,yp,zp;
  xp = yp = zp = 0.0;
  for(int i = 0; i < 5000; ) {
    int numrates = gyro.getRates(256, rates);
    xa = ya = za = 0;        
    if(numrates > 0) {
      for(int j = 0; j < numrates; j++) {
	xa = rates[j].x;
	ya = rates[j].y;
	za = rates[j].z;
	std::cout << std::dec << (i += numrates) << " "
		<< numrates << " "
		<< rates[j].seq_no << " "
		<< xa << " " << ya << " " << za << "\n";      
	std::cout.flush();
      }
    }
    usleep(100);
  }
}
