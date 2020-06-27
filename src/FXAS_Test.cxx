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
    std::cerr << "Creating pigpio client\n";
    piio_p = new BlackBox::PIIOD;
  }
  else {
    std::cerr << "Creating pigpio RAW\n";    
    piio_p = new BlackBox::PIIORaw; 
  }

  std::cerr << "Created piio\n";
  
  BlackBox::FXAS21002C gyro(piio_p, 1, 0x21, 4, BlackBox::FXAS21002C::FIFO);

  std::cerr << "Created gyro\n";
  
  //  gyro.start(BlackBox::FXAS21002C::CR1_DATA_RATE_25);
  gyro.start(BlackBox::FXAS21002C::CR1_DATA_RATE_25);

  std::cerr << "Started gyro\n";
  
  BlackBox::Rates rates[256];

  std::cerr << "Allocated rates\n";

  float xa, ya, za;
  xa = ya = za = 0.0;
  
  int xcorr, ycorr, zcorr;
  xcorr = ycorr = zcorr = 0;
  // ignore the first second or so as the device settles.
  int i; 
  for(i = 0; i < 1000; ) { //1000; ) {
    int numrates = gyro.getRates(256, rates);
    i += numrates;
  }

  std::cout << "Through warmup.\n"; std::cout.flush();
  
  float xp,yp,zp;
  xp = yp = zp = 0.0;
  for(int i = 0; i < 1000; ) {
    int numrates = gyro.getRates(256, rates);
    xa = ya = za = 0;        
    if(numrates > 0) {
      for(int j = 0; j < numrates; j++) {
	xa = rates[j].x;
	ya = rates[j].y;
	za = rates[j].z;
	xp += xa;
	yp += ya;
	zp += za;
	std::cout << std::dec << (i++) << " "
		<< numrates << " "
		<< rates[j].seq_no << " "
		<< xa << " " << ya << " " << za << " "
		<< xp << " " << yp << " " << zp << "\n";      	
	std::cout.flush();
	
      }
    }
    //    usleep(100);
  }
}
