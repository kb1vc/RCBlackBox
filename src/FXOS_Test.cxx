#include "FXOS8700CQ.hxx"
#include "Lamp.hxx"
#include <iostream>
#include <unistd.h>
#include <boost/format.hpp>
#include "PIIORaw.hxx"
#include "PIIOD.hxx"

void doPoll(BlackBox::FXOS8700CQ & comp, BlackBox::Lamp & lamp) {
  BlackBox::MXData dat;

  for(int i = 0; i < 512; ) {
    int nummx = comp.readDR(dat);
    if((i % 32) == 0) {
      if((i / 32) & 1) {
	lamp.on(); 
      }
      else { 
	lamp.off();    
      }
    }
    if(nummx) {
      std::cout << boost::format("%4d: %2d %4d M: [%04x %04x %04x] A: [%04x %04x %04x]   M: [%4d %4d %4d] A: [%4d %4d %4d]\n")
	% i++ % nummx % dat.seq_no 
	% dat.mx % dat.my % dat.mz
	% dat.ax % dat.ay % dat.az	
	% dat.mx % dat.my % dat.mz
	% dat.ax % dat.ay % dat.az;
    }
    usleep(10000);
  }
}

void doInt(BlackBox::FXOS8700CQ & comp, BlackBox::Lamp & lamp) {
  BlackBox::MXData dat[256];

  for(int i = 0; i < 200; ) {
    if(i % 2) lamp.on(); else lamp.off();
    int nummx = comp.getMX(256, dat);
    for(int j = 0; j < nummx; j++) {
      std::cout << std::dec << i++ << " "
		<< nummx << " "
		<< dat[j].seq_no << " "
		<< dat[j].mx << " " << dat[j].my << " " << dat[j].mz << " "
		<< dat[j].ax << " " << dat[j].ay << " " << dat[j].az << "\n";
    }

    usleep(10000);
  }
}


int main(int argc, char ** argv) {
  // create a connection. We're going to use
  // GPIO 17 for the mag/acc interrupt
  BlackBox::PIIO * piio_p; 
  BlackBox::Lamp * lamp; 
  if((argc > 1) && (argv[1][0] == 'd')) {
    piio_p = new BlackBox::PIIOD;
  }
  else {
    piio_p = new BlackBox::PIIORaw; 
  }

  lamp = new BlackBox::Lamp(piio_p, 27, 64);

  lamp->on();
  
  if(0) {
    BlackBox::FXOS8700CQ comp(piio_p, 1, 0x1F, 23, BlackBox::FXOS8700CQ::DR_POLL);
    comp.start();    
    doPoll(comp, * lamp);
  }
  else {
    BlackBox::FXOS8700CQ comp(piio_p, 1, 0x1F, 23, BlackBox::FXOS8700CQ::DR_INT);
    comp.start();
    doInt(comp, *lamp);
  }
  // BlackBox::PIIOD piio;  



}
