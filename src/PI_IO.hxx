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

    bool getState() const;

    void setDebounceInterval(unsigned int us_delay);
    
    void setHighCB(std::function<void(unsigned int, void*)> call_back, void * userptr = NULL);
    void setLowCB(std::function<void(unsigned int, void*)> call_back, void * userptr = NULL);
    void setTogleCB(std::function<void(unsigned int, void*)> call_back, void * userptr = NULL);

  protected:
    static void dispatchCallBack(int gpio, int level, unsigned int tick, void * sp);

    void switchEventCallBack(unsigned int tick, int level);
    
    enum CBKey { HIGH, LOW, TOGGLE };
    struct CBEl {
      CBEl(std::function<void(unsigned int, void*)> cb, void *up) : 
	call_back(cb), userptr(up) { }
      std::function<void(unsigned int, void*)> call_back;
      void * userptr; 
    };      
     
    std::map<CBKey, std::list<CBEl>> callback_map;
    
    void setupCallBack(CBKey key, std::function<void(unsigned int, void*)> call_back, void * userptr);

    void runCallBacks(unsigned int tick);

    unsigned int pin_num;
    bool pin_state;
    
    };
}

