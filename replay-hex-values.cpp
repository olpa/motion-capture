#include <string>
#include <iostream>
#include <fstream>
#include "measurement.hpp"

int main(int argc, char** argv) {
  std::string fname;
  if (argc < 2) {
    std::cerr << "Usage: replay-hex-value <file_name>" << std::endl;
    return 1;
  }
  fname = argv[1];
  //std::cout << "File name: " << fname << std::endl;
  std::ifstream is(fname);
  Measurement m;
  while (is) {
    is >> m;
    if (is.fail()) {
      break;
    }
    std::cout << "Got: " << m << std::endl;
  }
}
