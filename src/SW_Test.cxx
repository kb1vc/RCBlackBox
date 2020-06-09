#include <iostream>
#include <pigpio.h>
#include "PIIORaw.hxx"
#include "PIIOD.hxx"
#include "Switch.hxx"

int main() {
  //  BlackBox::PIIORaw piio;
  BlackBox::PIIOD piio;   

  const int sw_pin = 14;  
  BlackBox::Switch sw(&piio, sw_pin, 50000);

  const int led_pin = 15;
  
  piio.setMode(led_pin, PI_OUTPUT);

  piio.setPWMRange(led_pin, 255);
  piio.setPWMFrequency(led_pin, 0);
  piio.setPWM(led_pin, 0);
	
  bool v = false;
  while(1) {
    bool nv = sw.getState();
    if(nv != v) {
      std::cerr << "Toggled!  Now " << (nv ? "HIGH" : "LOW") << "\n";
      v = nv;
      if(!nv) {
	piio.setPWM(led_pin, 32);
      }
      else {
	piio.setPWM(led_pin, 0);
      }
    }
  }
}
