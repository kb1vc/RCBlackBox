#include "FXAS21002C.hxx"
#include "FXOS8700CQ.hxx"
#include <iostream>
#include <unistd.h>
#include "PIIOD.hxx"
#include "PIIORaw.hxx"

// Flight Data Recorder based on FXAS21002C (gyro, accelerometer) and FXOS8700CQ (compass)


int main(int argc, char * argv[]) {
  // create a connection. We're going to use
  // GPIO 4 and 17 for the interrupts. (4 for the Gyro FIFO interrupt, 17)
  BlackBox::PIIO * piio_p; 

  // This supports either the daemon interface, or the raw interface. 
  if((argc > 1) && (argv[1][0] == 'd')) {
    std::cerr << "Creating pigpio client\n";
    piio_p = new BlackBox::PIIOD;
  }
  else {
    std::cerr << "Creating pigpio RAW\n";    
    piio_p = new BlackBox::PIIORaw; 
  }

  // setup the gyro device. 
  BlackBox::FXAS21002C gyro(piio_p, 1, 0x21, 4, BlackBox::FXAS21002C::FIFO);
  std::cerr << "Created gyro\n";
  BlackBox::FXOS8700CQ compass(piio_p, 1, 0x1f, 17, BlackBox::FXOS8700CQ::DR_POLL);
  std::cerr << "Created compass\n";

  const int sw_pin = 14;
  const int led_pin = 15;  
  BlackBox::Switch sw(piio_p, sw_pin, 50000);
  BlackBox::Lamp led(piio_p, led_pin, 8);

  // open the fdr object.
  BlackBox::BlackBox fdr(gyro, compass, sw);
  
  // start the devices
  fdr.run();
  
  gyro.start(BlackBox::FXAS21002C::CR1_DATA_RATE_25);
  compass.start();
  
  std::cerr << "Started gyro and compass\n";
  
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
