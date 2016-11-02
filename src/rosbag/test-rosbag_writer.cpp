#include <string>
#include <fstream>
#include <ios>
#include <gmock/gmock.h>
using namespace testing;

void write_initial_line(std::ostream& os) {
  os << "#ROSBAG V2.0\x0a";
}

class SampleData {
public:
  SampleData() {
    std::stringstream ss;
    std::ifstream f("../../ros/sample_tf_events_only_2.bag", std::ios_base::in | std::ios_base::binary);
    ss << f.rdbuf();
    bytes_ = ss.str();
  }

  std::string initial_line() const {
    return bytes_.substr(0, 13);
  }
private:
  std::string bytes_;
};

SampleData const static_sample;

struct Writer : public Test {
  SampleData sample = static_sample;
};

TEST_F(Writer, Writes_Initial_Line) {
  std::stringstream ss;

  write_initial_line(ss);

  ASSERT_THAT(ss.str(), Eq(sample.initial_line()));
}

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
