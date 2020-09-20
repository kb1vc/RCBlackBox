#pragma once 
// Flight Data Recorder based on FXAS21002C (gyro, accelerometer) and FXOS8700CQ (compass)
#include <string>
#include <iostream>
#include <fstream>

namespace BlackBox {
  class PIIO;
  class FXAS21002C;
  class FXOS8700CQ;
  class Switch;
  class Lamp;
  class Video;
  
  class FDR {
  public:
    FDR(bool use_piio_server, bool use_camera, bool use_gyro);

    std::ostream & openLog(const std::string & fname);

    void closeLog();

    void run();

  protected:
    std::string getTimeStamp();

    std::string getTimeDate();

    const int GYRO_INT_PIN = 4;
    const int GYRO_I2C_ADDR = 0x21;
    const int COMPASS_INT_PIN = 23;
    const int COMPASS_I2C_ADDR = 0x1f;
    const int LED_OUT_PIN = 27;
    const int SWITCH_IN_PIN = 22;
    
    BlackBox::PIIO * piio_p;
  
    BlackBox::FXAS21002C * gyro_p;
    BlackBox::FXOS8700CQ * compass_p; 
    BlackBox::Switch * sw_p;
    BlackBox::Lamp * led_p;
    BlackBox::Video * video_p;
    
    std::ofstream log_stream;
    std::string video_fname; 
  };
}
