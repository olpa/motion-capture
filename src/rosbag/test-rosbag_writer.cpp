#include <string>
#include <iostream>
#include <fstream>
#include <ios>
#include <gmock/gmock.h>
using namespace testing;

struct Header {
  uint32_t chunk_count;
  uint32_t conn_count;
  uint64_t index_pos;
  uint8_t  op;
};

// Let's hope the host is little-endian
// There are htonl() etc for big-endian (network),
// but no equivalent for little-endian.
//
struct IoUInt32 {
  IoUInt32(uint32_t value) : value_(value) {}
  uint32_t value_;
};

std::ostream& operator<<(std::ostream& os, IoUInt32 const& v) {
  os.write(reinterpret_cast<char const*>(&v.value_), 4);
  return os;
}

struct IoUInt64 {
  IoUInt64(uint64_t value) : value_(value) {}
  uint64_t value_;
};

std::ostream& operator<<(std::ostream& os, IoUInt64 const& v) {
  os.write(reinterpret_cast<char const*>(&v.value_), 8);
  return os;
}
void write_initial_line(std::ostream& os) {
  os << "#ROSBAG V2.0\x0a";
}

void write_header(std::ostream& os, Header const& h) {
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

  Header get_header_obj() const {
    Header h;
    h.chunk_count = 1;
    h.conn_count = 1;
    h.index_pos = 6489;
    h.op = 3;
    return h;
  }

  std::string header() const {
    return bytes_.substr(17, 86-17);
  }

private:
  std::string bytes_;
};

struct Serializer : public Test {
  std::stringstream ss;
};

TEST_F(Serializer, Int32) {
  IoUInt32 v(0x44434241u); // chr('A') = 0x41

  ss << v;

  ASSERT_THAT(ss.str(), Eq(std::string("ABCD")));
}

TEST_F(Serializer, Int64) {
  IoUInt64 v(0x3332313044434241ull); // chr('0') = 0x30

  ss << v;

  ASSERT_THAT(ss.str(), Eq(std::string("ABCD0123")));
}


SampleData const static_sample;

struct Writer : public Test {
  SampleData sample = static_sample;
  std::stringstream ss;
};

TEST_F(Writer, Writes_Initial_Line) {
  void();

  write_initial_line(ss);

  ASSERT_THAT(ss.str(), Eq(sample.initial_line()));
}

TEST_F(Writer, DISABLED_Writes_Header) {
  Header h = sample.get_header_obj();

  write_header(ss, h);

  ASSERT_THAT(ss.str(), Eq(sample.header()));
}

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
