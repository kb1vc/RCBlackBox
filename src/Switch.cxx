#include <iostream>
#include <sys/time.h>
#include <stdexcept>
#include "PIIO.hxx"
#include "Switch.hxx"
#include <pigpio.h>
#include <unistd.h>

namespace BlackBox {

  void Switch::setDebounceInterval(unsigned int us_delay) {
    piio_p->setGlitchFilter(pin_num, us_delay);
  }
  
  // a simple debounced switch
  Switch::Switch(PIIO * piio_p, 
		 unsigned int pin_num, unsigned int us_delay) : piio_p(piio_p), pin_num(pin_num) {
    setDebounceInterval(us_delay);

    piio_p->setMode(pin_num, PI_INPUT);
    piio_p->setPullUpDown(pin_num, PI_PUD_UP);
    
    
    // setup the callback.
    int stat = piio_p->setPinCallBack(pin_num, dispatchCallBack, this);

    // init pin state
    pin_state = piio_p->read(pin_num);
  }

  void Switch::setHighCB(std::function<void(unsigned int, void*)> call_back, void * userptr) {
    setupCallBack(HIGH, call_back, userptr);
  }
  
  void Switch::setLowCB(std::function<void(unsigned int, void*)> call_back, void * userptr) {
    setupCallBack(LOW, call_back, userptr);
  }
  
  void Switch::setTogleCB(std::function<void(unsigned int, void*)> call_back, void * userptr)  {
    setupCallBack(TOGGLE, call_back, userptr);
  }

  void Switch::Switch::setupCallBack(CBKey key, std::function<void(unsigned int, void*)> call_back, void * userptr) {
    CBEl el(call_back, userptr);
    callback_map[key].push_back(el);
  }

  void Switch::runCallBacks(unsigned int tick) {
    CBKey key = pin_state ? HIGH : LOW;

    for(auto ce : callback_map[key]) {
      ce.call_back(tick, ce.userptr);
    }
    for(auto ce : callback_map[TOGGLE]) {
      ce.call_back(tick, ce.userptr);
    }
  }

  bool Switch::getState() const {
    return pin_state;
  }  

  void Switch::dispatchCallBack(int pin, int level, unsigned int tick, void * sp) {
    ((Switch *) sp)->switchEventCallBack(tick, level);
  }

  void Switch::switchEventCallBack(unsigned int tick, int level) {
    pin_state = (level != 0);
    runCallBacks(tick);
  }

  bool Switch::waitForSwitch(bool state, unsigned int sleep_interval) {
    while(getState() != state) {
      usleep(sleep_interval);
    }
    
    return getState();
  }
}
