#include <pigpio.h>
#include "PI_IO.hxx"
#include <iostream>
#include <sys/time.h>
#include <stdexcept>

static bool pigpio_has_been_initialized = false;
namespace BlackBox {
  bool initPIGPIO() {
    if(pigpio_has_been_initialized) return true;

    // two peoples separated by a common language. 
    int stat = gpioInitialise();
    
    std::cerr << "PIGPIO initialized returns " << stat << "\n";

    // setup debug
    //    gpioCfgInternals(984762879, 7);
    
    return pigpio_has_been_initialized = (stat > 0);
  }

  bool closePIGPIO() {
    if(pigpio_has_been_initialized) gpioTerminate();
    pigpio_has_been_initialized = false; 
    return true; 
  }


  // a simple debounced switch
  Switch::Switch(unsigned int pin_num, unsigned int us_delay) : pin_num(pin_num) {
    setDebounceInterval(us_delay);
    
    // setup the callback.
    int stat = gpioSetISRFuncEx(pin_num, EITHER_EDGE, 0, dispatchCallBack, this);
  }

  bool Switch::readPin() {
    int v = gpioRead(pin_num);
    if(v == PI_BAD_GPIO) {
      throw std::runtime_error("Error reading pin.");
    }

    return v != 0;
  }

  void Switch::setHighCB(std::function<void(void*)> call_back, void * userptr) {
    setupCallBack(HIGH, call_back, userptr);
  }
  
  void Switch::setLowCB(std::function<void(void*)> call_back, void * userptr) {
    setupCallBack(LOW, call_back, userptr);
  }
  
  void Switch::setTogleCB(std::function<void(void*)> call_back, void * userptr)  {
    setupCallBack(TOGGLE, call_back, userptr);
  }

  void Switch::Switch::setupCallBack(CBKey key, std::function<void(void*)> call_back, void * userptr) {
    CBEl el(call_back, userptr);
    callback_map[key].push_back(el);
  }

  void Switch::runCallBacks() {
    CBKey key = pin_state ? HIGH : LOW;

    for(auto ce : callback_map[key]) {
      ce.call_back(ce.userptr);
    }
    for(auto ce : callback_map[TOGGLE]) {
      ce.call_back(ce.userptr);
    }
  }
  
  void Switch::dispatchCallBack(int gpio, int level, unsigned int tick, void * sp) {
    ((Switch *) sp)->switchEventCallBack(level);
  }

  void Switch::switchEventCallBack(int level) {
    bool v = (level != 0);
    if((v ^ pin_state) && timeExpired(last_transition)) {
      gettimeofday(&last_transition, NULL);

      pin_state = v;
      runCallBacks();
    }
  }
  bool Switch::timeExpired(timeval & old) {
    struct timeval now;
    gettimeofday(&now, NULL);
    struct timeval diff;
    timersub(&now, &old, &diff);
    return (diff.tv_sec > 0) || (diff.tv_usec > debounce_interval);
  }
}
