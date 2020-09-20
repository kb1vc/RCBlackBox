
#include "FXOS8700CQ.hxx"
#include <iostream>
#include <unistd.h>
#include "PIIORaw.hxx"
#include "Switch.hxx"
#include "Lamp.hxx"

int main(int argc, char * argv[]) {
  // create a connection. We're going to use
  // GPIO 4 and 17 for the interrupts. (4 for the Gyro FIFO interrupt, 17)
  // open the fdr object.
  BlackBox::PIIORaw piio;
  BlackBox::FXOS8700CQ compass(&piio, 1, 0x1f, 17, BlackBox::FXOS8700CQ::DR_INT);

  const int sw_pin = 14;
  BlackBox::Switch sw(&piio, sw_pin, 50000);
  
  const int led_pin = 15;
  BlackBox::Lamp led(&piio, led_pin, 8);

  led.off();

  BlackBox::MXData bearings[256];
  
  compass.start();
  
  while(1) {
    sw.waitForSwitch(false);

    // get a compass reading.
    int num_b;
    int last_idx = 0;
    while((num_b = compass.getMX(256, bearings)) != 0) {
      last_idx = num_b; 
    }
    if(last_idx == 0) {
      std::cout << "Ooops\n";
    }
    else {
      bearings[last_idx - 1].print(std::cout);
    }
    
    sw.waitForSwitch(true);
  }
  
}
