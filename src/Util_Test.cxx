#include "Utils.hxx"
#include <iostream>
#include <string>

int main() {
  std::cout << BlackBox::timeToFilename("test_", ".log") << "\n";
}
