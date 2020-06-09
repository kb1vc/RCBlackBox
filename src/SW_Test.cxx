#include <iostream>
#include <pigpio.h>
#include "PI_IO.hxx"

int main() {
  BlackBox::initPIGPIO();
  const int sw_pin = 14;  
  BlackBox::Switch sw(sw_pin, 50000);

  const int led_pin = 15;
  
  gpioSetMode(led_pin, PI_OUTPUT);
  gpioSetPWMrange(led_pin, 255);
  gpioSetPWMfrequency(led_pin, 0);
  gpioPWM(led_pin, 0);
	
  bool v = false;
  while(1) {
    bool nv = sw.getState();
    if(nv != v) {
      std::cerr << "Toggled!  Now " << (nv ? "HIGH" : "LOW") << "\n";
      v = nv;
      if(!nv) {
	gpioPWM(led_pin, 32);
      }
      else {
	gpioPWM(led_pin, 0);
      }
    }
  }
}
