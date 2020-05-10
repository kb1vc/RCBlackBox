#pragma once

/**
 * @brief Initialize PIGPIO system, but do it only once. 
 * 
 * @return true if all was well, false otherwise. 
 */
namespace BlackBox { 
  bool initPIGPIO();
}

