#include <iostream>
#include <sys/time.h>
#include <stdexcept>
#include "PIIO.hxx"
#include "Lamp.hxx"
#include <pigpio.h>

namespace BlackBox {

  // a simple LED lamp
  Lamp::Lamp(PIIO * piio_p, 
	     unsigned int pin_num, 
	     unsigned int _brightness) : 
    piio_p(piio_p), pin_num(pin_num) {
    
    initLamp();

    setBrightness(_brightness);
  }

  void Lamp::initLamp() {
    piio_p->setMode(pin_num, PI_OUTPUT);

    piio_p->setPWMRange(pin_num, 255);

    piio_p->setPWMFrequency(pin_num, 100);

    off();
  }

  void Lamp::setBrightness(unsigned int _brightness) {
    brightness = (_brightness > 20) ? 20 : _brightness; 
  }

  bool Lamp::blink(bool blink_en) {
    if(blink_en) {
      std::cerr << "Blink on\n";
      piio_p->setPWMFrequency(pin_num, 0);
      piio_p->setPWM(pin_num, 16);
    }
    else {
      std::cerr << "Blink off\n";      
      piio_p->setPWMFrequency(pin_num, 100);
    }
    return true;
  }
  
  bool Lamp::setState(bool st) {
    blink(false);
    bool old_state = lamp_state; 
    lamp_state = st;
    piio_p->setPWM(pin_num, st ? brightness : 0);
    return old_state; 
  }

  bool Lamp::getState() { return lamp_state; }
    
  bool Lamp::on() {
    return setState(true);
  }

  bool Lamp::off() {
    return setState(false);
  }

}
