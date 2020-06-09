#include "FXOS8700CQ.hxx"
#include <iostream>
#include <unistd.h>
#include <boost/format.hpp>
#include "PIIORaw.hxx"
#include "PIIOD.hxx"

void doPoll(BlackBox::FXOS8700CQ & comp) {
  BlackBox::MXData dat;

  for(int i = 0; i < 2000; ) {
    int nummx = comp.readDR(dat);
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

void doInt(BlackBox::FXOS8700CQ & comp) {
  BlackBox::MXData dat[256];

  for(int i = 0; i < 2000; ) {
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


int main() {
  // create a connection. We're going to use
  // GPIO 17 for the mag/acc interrupt
  BlackBox::PIIOD piio;
  BlackBox::FXOS8700CQ comp(&piio, 1, 0x1F, 17, BlackBox::FXOS8700CQ::DR_POLL);
  //BlackBox::FXOS8700CQ comp(1, 0x1F, 17, BlackBox::FXOS8700CQ::DR_INT);  

  comp.start();
  doPoll(comp);
  // doInt(comp);
}
