#include "FXOS8700CQ.hxx"
#include <iostream>
#include <unistd.h>

void doPoll(BlackBox::FXOS8700CQ & comp) {
  BlackBox::MXData dat;

  for(int i = 0; i < 2000; ) {
    int nummx = comp.readDR(dat);
    if(nummx) {
      std::cout << std::dec << i++ << " "
		<< nummx << " "
		<< dat.seq_no << " " << std::hex
		<< dat.mx << " " << dat.my << " " << dat.mz << " "
		<< dat.ax << " " << dat.ay << " " << dat.az << "\n";
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

  BlackBox::FXOS8700CQ comp(1, 0x1F, 17, BlackBox::FXOS8700CQ::DR_POLL);
  //BlackBox::FXOS8700CQ comp(1, 0x1F, 17, BlackBox::FXOS8700CQ::DR_INT);  

  comp.start();
  doPoll(comp);
  // doInt(comp);
}
