#include "FXAS21002C.hxx"
#include <iostream>

int main() {
  // create a connection. We're going to use
  // GPIO 4 and 17 for the interrupts. (4 for the FIFO interrupt.)

  BlackBox::FXAS21002C gyro(1, 21, 4, 17);

  gyro.init(BlackBox::FXAS21002C::FIFO);

  BlackBox::FXAS21002C::Rates rates[256];
  
  for(i = 0; i < 10; i++) {
    int numrates = gyro.getRates(256, rates);

    if(numrates > 0) {
      std::cout << i << " " 
		<< numrates << " " 
		<< rates[0].seq_no << " "
		<< rates[0].x << " " 
		<< rates[0].y << " "
		<< rates[0].z << "\n";
    }
  }
}
