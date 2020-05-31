#pragma once
#include <map>
#include <functional>
#include <list>

/**
 * @brief Initialize PIGPIO system, but do it only once. 
 * 
 * @return true if all was well, false otherwise. 
 */

struct timeval;

namespace BlackBox { 
  bool initPIGPIO();

  class Switch {
  public:
    Switch(unsigned int pin_num, unsigned int us_delay = 10000); // 10mS debounce time

    bool getState() const { return pin_state; }


    void setDebounceInterval(unsigned int us_delay) { debounce_interval = us_delay; }
    
    void setHighCB(std::function<void(void*)> call_back, void * userptr = NULL);
    void setLowCB(std::function<void(void*)> call_back, void * userptr = NULL);
    void setTogleCB(std::function<void(void*)> call_back, void * userptr = NULL);

  protected:
    bool readPin();
    
    static void dispatchCallBack(int gpio, int level, unsigned int tick, void * sp);

    void switchEventCallBack(int level);
    
    enum CBKey { HIGH, LOW, TOGGLE };
    struct CBEl {
      CBEl(std::function<void(void*)> cb, void *up) : 
	call_back(cb), userptr(up) { }
      std::function<void(void*)> call_back;
      void * userptr; 
    };      
     
    std::map<CBKey, std::list<CBEl>> callback_map;
    
    void setupCallBack(CBKey key, std::function<void(void*)> call_back, void * userptr);

    void runCallBacks();

    bool timeExpired(timeval & old);
  
    unsigned int pin_num;
    timeval last_transition;
    unsigned long debounce_interval;
    bool pin_state;
    
    };
}

