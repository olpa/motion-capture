#include <string>
#include <iostream>
#include <fstream>
#include <ios>
#include <functional>
#include <gmock/gmock.h>
using namespace testing;

struct Header {
  uint32_t chunk_count;
  uint32_t conn_count;
  uint64_t index_pos;
  uint8_t  op;
};

void write_initial_line(std::ostream& os) {
  os << "#ROSBAG V2.0\x0a";
}

// Let's hope the host is little-endian
// There are htonl() etc for big-endian (network),
// but no equivalent for little-endian.
//
void write_value(std::ostream& os, uint8_t const v) {
  os << v;
}

void write_value(std::ostream& os, std::string const& v) {
  os << v;
}

void write_value(std::ostream& os, uint32_t const v) {
  os.write(reinterpret_cast<char const*>(&v), 4);
}

void write_value(std::ostream& os, uint64_t const v) {
  os.write(reinterpret_cast<char const*>(&v), 8);
}

void write_with_length_prefix(std::ostream& os, std::function<void()> core_func) {
  auto pos1 = os.tellp();
  write_value(os, static_cast<uint32_t>(0));
  core_func();
  auto pos2 = os.tellp();
  uint32_t len = static_cast<uint32_t>(pos2 - pos1 - 4);
  os.seekp(pos1);
  write_value(os, len);
  os.seekp(pos2);
}

template<class T>
void write_key_value(std::ostream& os, std::string const& key, T const& value) {
  auto core_func = [&](){
    os << key << '=';
    write_value(os, value);
  };
  write_with_length_prefix(os, core_func);
}

void write_header(std::ostream& os, Header const& h) {
  auto core_func = [&](){
    write_key_value(os, "chunk_count", h.chunk_count);
    write_key_value(os, "conn_count",  h.conn_count);
    write_key_value(os, "index_pos",   h.index_pos);
    write_key_value(os, "op",          h.op);
  };
  write_with_length_prefix(os, core_func);
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
    return bytes_.substr(13, 86-13);
  }

private:
  std::string bytes_;
};

struct Serializer : public Test {
  std::stringstream ss;
};

TEST_F(Serializer, Int8) {
  uint8_t v('Q');

  write_value(ss, v);

  ASSERT_THAT(ss.str(), Eq(std::string("Q")));
}

TEST_F(Serializer, Int32) {
  uint32_t v(0x44434241u); // chr('A') = 0x41

  write_value(ss, v);

  ASSERT_THAT(ss.str(), Eq(std::string("ABCD")));
}

TEST_F(Serializer, Int64) {
  uint64_t v(0x3332313044434241ull); // chr('0') = 0x30

  write_value(ss, v);

  ASSERT_THAT(ss.str(), Eq(std::string("ABCD0123")));
}

TEST_F(Serializer, KeyAndStringValue) {
  void();

  write_key_value(ss, "key", "value");

  ASSERT_THAT(ss.str(), Eq(std::string("\x09\x00\x00\x00key=value", 4+9)));
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

TEST_F(Writer, Writes_Header) {
  Header h = sample.get_header_obj();

  write_header(ss, h);

  ASSERT_THAT(ss.str(), Eq(sample.header()));
}

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
