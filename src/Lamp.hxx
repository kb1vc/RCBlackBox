#pragma once
#include <map>
#include <functional>
#include <list>

/**
 * @brief Manage an LED on a selected pin.
 * 
 */

namespace BlackBox {
  class PIIO; 
  class Lamp {
  public:
    Lamp(PIIO * piio_p, 
	 unsigned int pin_num, 
	 unsigned int _brightness = 10); 

    void setBrightness(unsigned int _brightness); 

    bool setState(bool st);

    bool getState();
    
    bool on();

    bool off();

    bool blink(bool blink_en = true);

  protected:
    PIIO * piio_p;

    void initLamp();

    
    unsigned int pin_num;
    bool lamp_state;
    unsigned int brightness;
  };
}

