#include "FXAS21002C.hxx"
#include "FXOS8700CQ.hxx"
#include "Lamp.hxx"
#include "Switch.hxx"
#include "Video.hxx"
#include "PIIOD.hxx"
#include "PIIORaw.hxx"
#include "FDR.hxx"
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <chrono>



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
    led_p->off();

    // start the video process.
    video_p = new BlackBox::Video();
  }


  void FDR::run() {
    // wait for the start button.
    sw_p->waitForSwitch(false); 
    sw_p->waitForSwitch(true);

    
    std::cerr << "Ready to start\n";

    // show that we're running. 
    led_p->blink(true);

    // start the gyro
    gyro_p->start(BlackBox::FXAS21002C::CR1_DATA_RATE_25);
    // start the compass
    compass_p->start();
    std::cerr << "Started gyro and compass\n";
  
  
    BlackBox::Rates rates[256];
    BlackBox::MXData bearings[256];

    video_p->openVidFile(video_fname);

    // start the video
    video_p->start();
    
    // loop
    while(sw_p->getState()) {
      // wait for gyro buffer
      int num_rates = gyro_p->getRates(256, rates);
      // read compass buffer if available
      int num_bearings = compass_p->getMX(256, bearings);

      // print the records. 
      if((num_rates | num_bearings) != 0) {
	// time stamp
	log_stream << getTimeStamp() << "\n";
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
    led_p->off();

    // stop the video
    video_p->stop();

    // stop the gyro and compass
    gyro_p->stop();
    compass_p->stop();
  }

  std::ostream & FDR::openLog(const std::string & basename) {
    std::string td = getTimeDate();
    std::string fname = basename + td + ".fdr_log";
    video_fname = basename + td + ".h264";
    log_stream.open(fname);
    log_stream << "FMT TS sec msec\n";
    BlackBox::MXData::printFormat(log_stream);
    BlackBox::Rates::printFormat(log_stream);

    std::cout << "Opening video file [" << video_fname << "]\n";
    std::cout << "Opening log file [" << fname << "]\n";
    std::cout.flush();
    return log_stream;
  }

  std::string FDR::getTimeStamp() {
    std::stringstream ss;
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    
    unsigned long msec = t.tv_nsec / 1000000;
    ss << t.tv_sec <<  " " << msec;
    return ss.str();
  }

  std::string FDR::getTimeDate() {
    std::stringstream ss;
    using std::chrono::system_clock; 
    std::time_t tt = system_clock::to_time_t(system_clock::now());
    struct std::tm * ptm = std::localtime(&tt);
    
    ss << std::put_time(ptm, "%y-%m-%d_%OH_%OM_%OS");
    return ss.str();
  }
}
