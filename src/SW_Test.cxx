#include <iostream>
#include <pigpio.h>
#include "PIIORaw.hxx"
#include "PIIOD.hxx"
#include "Switch.hxx"
#include "Lamp.hxx"
#include <unistd.h>

int main() {
  BlackBox::PIIORaw piio;
  // BlackBox::PIIOD piio;   

  const int sw_pin = 22;
  const int led_pin = 27;  
  BlackBox::Switch sw(&piio, sw_pin, 50000);
  BlackBox::Lamp led(&piio, led_pin, 8);

  bool v = false;
  led.blink();

  std::cerr << "waiting on button press.\n";
  while(sw.getState()) {
    usleep(1000);
  }
  std::cerr << "got it.  waiting on release.\n";
  while(!sw.getState()) {
    usleep(1000);
  }
  std::cerr << "ready\n";
  
  unsigned int bright = 1;
  while(1) {
    bool nv = sw.getState();
    if(nv != v) {
      if(!v) {
	bright++; 
	led.setBrightness(bright);
      }
      
      std::cerr << "Toggled!  Now " << (nv ? "HIGH" : "LOW") << " bright " << bright << "\n";
      v = nv;
      led.setState(!v);
    }
  }
}
