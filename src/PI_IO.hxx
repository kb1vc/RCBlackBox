#pragma once
#include <map>
#include <functional>
#include <list>


namespace BlackBox {
  /**
   * @brief Initialize PIGPIO system, but do it only once. 
   * 
   * @return true if all was well, false otherwise. 
   */
  bool initPIGPIO();

  bool closePIGPIO();
}

