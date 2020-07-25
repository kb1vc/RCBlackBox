#include "FXAS21002C.hxx"
#include "FXOS8700CQ.hxx"
#include "Lamp.hxx"
#include "Switch.hxx"
#include <iostream>
#include <unistd.h>
#include "PIIOD.hxx"
#include "PIIORaw.hxx"

// Flight Data Recorder based on FXAS21002C (gyro, accelerometer) and FXOS8700CQ (compass)

namespace BlackBox { 
  FDR::FDR(bool use_piio_server) {
    if(use_piio_server) {
      std::cerr << "Creating pigpio client\n";
      piio_p = new BlackBox::PIIOD;
    }
    else {
      std::cerr << "Creating pigpio RAW\n";    
      piio_p = new BlackBox::PIIORaw; 
    }

  
    // setup the gyro device. 
    gyro_p = new BlackBox::FXAS21002C(piio_p, 1, 0x21, 4, BlackBox::FXAS21002C::FIFO);
    std::cerr << "Created gyro\n";
    compass_p = new BlackBox::FXOS8700CQ(piio_p, 1, 0x1f, 17, BlackBox::FXOS8700CQ::DR_INT);
    std::cerr << "Created compass\n";

    const int sw_pin = 14;
    const int led_pin = 15;  
    sw_p = new BlackBox::Switch(piio_p, sw_pin, 50000);
    led_p = new BlackBox::Lamp(piio_p, led_pin, 8);
    
    // turn off the lamp.
    led_p->clear();

    // start the video process.
    video_p = new BlackBox::Video();
  }


  void FDR::run() {
    // wait for the start button.
    sw_p->waitForSwitch(true); 
    sw_p->waitForSwitch(false);
  
    // show that we're running. 
    led_p->blink(true);

    // start the gyro
    gyro_p->start(BlackBox::FXAS21002C::CR1_DATA_RATE_25);
    // start the compass
    compass_p->start();
    std::cerr << "Started gyro and compass\n";
  
    // start the video
    video_p->start();
  
    BlackBox::Rates rates[256];
    BlackBox::MXData bearings[256];
  
    // loop
    int i = interval_counter;
    while(button_not_pressed) {
      // wait for gyro buffer
      int num_rates = gyro_p->getRates(256, rates);
      // read compass buffer if available
      int num_bearings = compass_p->getMX(256, bearings);

      // print the records. 
      if((num_rates | num_bearings) != 0) {
	// time stamp
	writeTimeStamp(log_stream);
	// write records.      
	for(int i = 0; i < num_rates; i++) {
	  rates[i].print(log_stream);
	}
	for(int i = 0; i < num_bearings; i++) {
	  bearings[i].print(log_stream);
	}

	// sync files
	log_stream.flush();
      }
    }

    // close the log
    log_stream.close();
  
    // turn off the lamp
    led_p->clear();

    // stop the video
    video_p->stop();

    // stop the gyro and compass
    gyro_p->stop();
    compass_p->stop();
  }

  std::ostream & openLog(const std::string & basename) {
    std::string fname = basename + getDate4FName() + ".fdr_log";

    log_stream.open(fname);
    log_stream << "FMT TS sec nsec\n";
    BlackBox::MXData::printFormat(log_stream);
    BlackBox::Rates::printFormat(log_stream);
  }
}
