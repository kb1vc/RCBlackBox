#ifndef FDR_HDR
#define FDR_HDR

// Flight Data Recorder based on FXAS21002C (gyro, accelerometer) and FXOS8700CQ (compass)

class FDR {
  FDR(bool use_piio_server);

  void openLog();

  void openLog(std::string fname);

  void closeLog();

  void run();
  
  BlackBox::PIIO * piio_p;
  
  BlackBox::FXAS21002C * gyro_p;
  BlackBox::FXOS8700CQ * compass_p; 
  BlackBox::Switch * sw_p;
  BlackBox::Lamp * led_p;
};

#endif
