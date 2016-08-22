#include <string>
#include <iostream>

int main(int argc, char** argv) {
  std::string fname;
  if (argc < 2) {
    std::cerr << "Usage: replay-hex-value <file_name>" << std::endl;
    return 1;
  }
  fname = argv[1];
  std::cout << "File name: " << fname << std::endl;
}
