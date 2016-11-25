#include <string>
#include <iostream>
#include <fstream>
#include <ios>
#include <functional>
#include <gmock/gmock.h>
using namespace testing;

struct Header {
  enum struct Opcodes {
    message_data = 2,
    bag_header   = 3,
    index_data   = 4,
    chunk        = 5,
    chunk_info   = 6,
    connection   = 7,
  };
  uint32_t chunk_count;
  uint32_t conn_count;
  uint32_t count;
  uint32_t ver;
  uint64_t index_pos;
  std::string topic;
  uint64_t time;
  uint32_t conn;
  uint32_t size;
  std::string compression;
  uint64_t start_time;
  uint64_t chunk_pos;
  uint64_t end_time;
  uint8_t  op;
};

// http://wiki.ros.org/ROS/Connection%20Header
// Only partial here
struct ConnectionHeader {
  std::string topic, message_definition, md5sum, type;
};

struct IndexEntry {
  uint64_t time;
  uint32_t offset;
};

struct ChunkInfoEntry {
  uint32_t conn;
  uint32_t count;
};

struct MessageDefinition {
  std::string topic;
  std::string type;
  std::string md5sum;
  std::string message_definition;
};

MessageDefinition get_msg_def_TransformStamped() {
  MessageDefinition m;
  m.topic = "/tf";
  m.message_definition = R"(geometry_msgs/TransformStamped[] transforms

================================================================================
MSG: geometry_msgs/TransformStamped
# This expresses a transform from coordinate frame header.frame_id
# to the coordinate frame child_frame_id
#
# This message is mostly used by the 
# <a href="http://wiki.ros.org/tf">tf</a> package. 
# See its documentation for more information.

Header header
string child_frame_id # the frame id of the child frame
Transform transform

================================================================================
MSG: std_msgs/Header
# Standard metadata for higher-level stamped data types.
# This is generally used to communicate timestamped data 
# in a particular coordinate frame.
# 
# sequence ID: consecutively increasing ID 
uint32 seq
#Two-integer timestamp that is expressed as:
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
# time-handling sugar is provided by the client library
time stamp
#Frame this data is associated with
# 0: no frame
# 1: global frame
string frame_id

================================================================================
MSG: geometry_msgs/Transform
# This represents the transform between two coordinate frames in free space.

Vector3 translation
Quaternion rotation

================================================================================
MSG: geometry_msgs/Vector3
# This represents a vector in free space. 

float64 x
float64 y
float64 z
================================================================================
MSG: geometry_msgs/Quaternion
# This represents an orientation in free space in quaternion form.

float64 x
float64 y
float64 z
float64 w


)";
  m.md5sum = "94810edda583a504dfda3829e70d7eec";
  m.type = "tf/tfMessage";
  return m;
}

struct RosMsgHeader {
  uint32_t    seq;
  uint64_t    stamp;
  std::string frame_id;
};

struct RosMsgVector3 {
  double x, y, z;
};

struct RosMsgQuaternion {
  double x, y, z, w;
};

struct RosMsgTransform {
  RosMsgVector3    translation;
  RosMsgQuaternion rotation;
};

struct RosMsgTransformStamped {
  RosMsgHeader    header;
  std::string     child_frame_id;
  RosMsgTransform transform;
};

void write_initial_line(std::ostream& os) {
  os << "#ROSBAG V2.0\x0a";
}

// Let's hope the host is little-endian
// There are htonl() etc for big-endian (network),
// but no equivalent for little-endian.
//
// Also, let's hope that:
// "double" of C++ uses 64 bit
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

void write_value(std::ostream& os, double const v) {
  os.write(reinterpret_cast<char const*>(&v), 8);
}

template<class T>
struct RosIoClass {
  RosIoClass(T const& value) : value_(value) { };
  T const& value_;
};

template<class T>
std::ostream& operator<<(std::ostream& os, RosIoClass<T> const& value) {
  write_value(os, value.value_);
  return os;
}

template<class T>
auto RosIO(T const& val) -> RosIoClass<T> {
  return RosIoClass<T>(val);
}

template<>
std::ostream& operator<<(std::ostream& os, RosIoClass<std::string> const& ros_s) {
  std::string const& s = ros_s.value_;
  os << RosIO(static_cast<uint32_t>(s.length())) << s;
  return os;
}

// http://stackoverflow.com/questions/16825351/
using pos_type = std::char_traits<char>::pos_type;

pos_type reserve_size_value(std::ostream& os) {
  auto pos = os.tellp();
  write_value(os, static_cast<uint32_t>(0));
  return pos;
}

void fixate_size_value(std::ostream& os, pos_type const pos) {
  auto pos2 = os.tellp();
  uint32_t size = static_cast<uint32_t>(pos2 - pos - 4);
  os.seekp(pos);
  write_value(os, size);
  os.seekp(pos2);
}

void write_with_length_prefix(std::ostream& os, std::function<void()> core_func) {
  auto pos = reserve_size_value(os);
  core_func();
  fixate_size_value(os, pos);
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
    Header::Opcodes op = static_cast<Header::Opcodes>(h.op);
    switch (op) {
      default:
        break;
      case Header::Opcodes::bag_header:
        write_key_value(os, "chunk_count", h.chunk_count);
        write_key_value(os, "conn_count",  h.conn_count);
        write_key_value(os, "index_pos",   h.index_pos);
        break;
      case Header::Opcodes::chunk:
        write_key_value(os, "size",        h.size);
        write_key_value(os, "compression", h.compression);
        break;
      case Header::Opcodes::connection:
        write_key_value(os, "topic",       h.topic);
        write_key_value(os, "conn",        h.conn);
        break;
      case Header::Opcodes::message_data:
        write_key_value(os, "time",        h.time);
        write_key_value(os, "conn",        h.conn);
        break;
      case Header::Opcodes::index_data:
        write_key_value(os, "count", h.count);
        write_key_value(os, "ver", h.ver);
        write_key_value(os, "conn",        h.conn);
        break;
      case Header::Opcodes::chunk_info:
        write_key_value(os, "count", h.count);
        write_key_value(os, "ver", h.ver);
        write_key_value(os, "start_time",  h.start_time);
        write_key_value(os, "chunk_pos",   h.chunk_pos);
        write_key_value(os, "end_time",    h.end_time);
        break;
    }
    write_key_value(os, "op", h.op);
  };
  write_with_length_prefix(os, core_func);
}

std::ostream& operator<<(std::ostream& os, Header const& h) {
  write_header(os, h);
  return os;
}

std::ostream& operator<<(std::ostream& os, ConnectionHeader const& ch) {
  if (ch.topic.length()) {
    write_key_value(os, "topic", ch.topic);
  }
  if (ch.message_definition.length()) {
    write_key_value(os, "message_definition", ch.message_definition);
  }
  if (ch.md5sum.length()) {
    write_key_value(os, "md5sum", ch.md5sum);
  }
  if (ch.type.length()) {
    write_key_value(os, "type", ch.type);
  }
  return os;
}

void write_message_definition(std::ostream& os, MessageDefinition const& m) {
  auto core_func = [&]() {
    write_key_value(os, "topic",  m.topic);
    write_key_value(os, "message_definition", m.message_definition);
    write_key_value(os, "md5sum", m.md5sum);
    write_key_value(os, "type",   m.type);
  };
  write_with_length_prefix(os, core_func);
}

void write_bag_header_record(std::ostream& os, Header const& h) {
  // According to the documentation, the magic constant is 4096
  // (the header is 4096 bytes long). However, according to the
  // sample bag-files, it is not so.
  const int magic_ros_constant = 4104;
  auto pos1 = os.tellp();
  write_header(os, h);
  auto used_length = os.tellp() - pos1;
  uint32_t padding_length = magic_ros_constant - used_length - 4;
  if (padding_length > magic_ros_constant) {
    // overfull, should never happen
    padding_length = 0;
  }
  write_value(os, padding_length);
  os << std::string(padding_length, ' ');
}

void write_connection_record(std::ostream& os, Header const& h, ConnectionHeader const& ch) {
  os << h;
  auto core_func = [&]() {
    os << ch;
  };
  write_with_length_prefix(os, core_func);
}

std::ostream& operator<<(std::ostream& os, RosMsgQuaternion const& m) {
  return os << RosIO(m.x) << RosIO(m.y) << RosIO(m.z) << RosIO(m.w);
}

std::ostream& operator<<(std::ostream& os, RosMsgVector3 const& m) {
  return os << RosIO(m.x) << RosIO(m.y) << RosIO(m.z);
}

std::ostream& operator<<(std::ostream& os, RosMsgTransform const& m) {
  return os << m.translation << m.rotation;
}

std::ostream& operator<<(std::ostream& os, RosMsgHeader const& m) {
  return os << RosIO(m.seq) << RosIO(m.stamp) << RosIO(m.frame_id);
}

std::ostream& operator<<(std::ostream& os, RosMsgTransformStamped const& m) {
  return os << m.header << RosIO(m.child_frame_id) << m.transform;
}

void write_message_raw_data(std::ostream& os, RosMsgTransformStamped const& m) {
  os << m;
}

template<class IteratorH, class IteratorM>
void write_messages(std::ostream& os, IteratorH begin, IteratorH end, IteratorM im) {
  for (auto i = begin; i != end; ++i) {
    write_header(os, *i);
    auto core_func = [&]() {
      write_value(os, (uint32_t)1);
      write_message_raw_data(os, *im++);
    };
    write_with_length_prefix(os, core_func);
  }
}

std::ostream& operator<<(std::ostream& os, IndexEntry const& ie) {
  os << RosIO(ie.time) << RosIO(ie.offset);
  return os;
}

std::ostream& operator<<(std::ostream& os, ChunkInfoEntry const& cie) {
  os << RosIO(cie.conn) << RosIO(cie.count);
  return os;
}

template<class Iterator>
void write_index_record(std::ostream& os, Header const& h, Iterator begin, Iterator end) {
  os << h;
  auto core_func = [&]() {
    for (auto i = begin; i != end; ++i) {
      os << *i;
    }
  };
  write_with_length_prefix(os, core_func);
}

template<class Iterator>
void write_chunk_info_record(std::ostream& os, Header const& h, Iterator begin, Iterator end) {
  os << h;
  auto core_func = [&]() {
    for (auto i = begin; i != end; ++i) {
      os << *i;
    }
  };
  write_with_length_prefix(os, core_func);
}

//

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

  std::string bag_header_record() const {
    return bytes_.substr(13, 4117-13);
  }

  Header get_chunk_header_obj() const {
    Header h;
    h.size = 2244;
    h.compression = "none";
    h.op = 5;
    return h;
  }

  MessageDefinition get_message_definition_obj() const {
    MessageDefinition m = get_msg_def_TransformStamped();
    return m;
  }

  std::string message_definition() const {
    return bytes_.substr(4204, 0x077e + 4);
  }

  std::vector<Header> get_tf_message_header_objects() const {
    Header h1;
    h1.time  = 1416593259237437348ul;
    h1.conn  = 0;
    h1.op    = 2;
    Header h2 = h1;
    h2.time = 1845739231743274916;
    return {h1, h2};
  }

  std::vector<RosMsgTransformStamped> get_tf_message_objects() const {
    RosMsgHeader h;
    h.seq  = 0;
    h.stamp = 1414530047142752164ul;
    h.frame_id = "world";
    RosMsgVector3 pos;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    RosMsgQuaternion rot; // 2nd: 0.65  0.8  0  1
    rot.x = 0.6;
    rot.y = 0.8999999999999999;
    rot.z = 0.14999999999999994;
    rot.w = 0.8;
    RosMsgTransform tf;
    tf.translation = pos;
    tf.rotation = rot;
    RosMsgTransformStamped m1, m2;
    m1.header = h;
    m1.child_frame_id = "imu_sensor1";
    m1.transform = tf;
    m2 = m1;
    m2.header.stamp = 1844095998730661796ul;
    m2.transform.rotation.x = 0.65;
    m2.transform.rotation.y = 0.7999999999999999;
    m2.transform.rotation.z = -5.551115123125783e-17;
    m2.transform.rotation.w = 1.0;
    return {m1, m2};
  }

  std::string tf_messages() const {
    return bytes_.substr(6126, 6410-6126);
  }

  Header get_connection_header_obj() {
    Header h;
    h.topic = "/tf";
    h.conn  = 0;
    h.op    = 7;
    return h;
  }

  ConnectionHeader get_connection_obj() {
    MessageDefinition m = get_msg_def_TransformStamped();
    ConnectionHeader ch;
    ch.topic = m.topic;
    ch.message_definition = m.message_definition;
    ch.md5sum = m.md5sum;
    ch.type = m.type;
    return ch;
  }

  std::string connection_record() const {
    return bytes_.substr(4166, 6126-4166);
  }

  Header get_index_header_obj() {
    Header h;
    h.count = 2;
    h.ver   = 1;
    h.conn  = 0;
    h.op    = 4;
    return h;
  }

  std::vector<IndexEntry> get_index_objects() {
    IndexEntry i1, i2;
    i1.time = 1416593259237437348ul;
    i1.offset = 1960;
    i2.time = 1845739231743274916;
    i2.offset = 2102;
    return {i1, i2};
  }

  std::string index_record() const {
    return bytes_.substr(6410, 6487-6408);
  }

  Header get_chunk_info_header_obj() {
    Header h;
    h.count = 1;
    h.ver   = 1;
    h.start_time = 1416593259237437348ul;
    h.chunk_pos  = 4117;
    h.end_time   = 1845739231743274916;
    h.op    = 6;
    return h;
  }

  std::vector<ChunkInfoEntry> get_chunk_info_objects() {
    ChunkInfoEntry cie;
    cie.conn  = 0;
    cie.count = 2;
    return {cie};
  }

  std::string chunk_info_record() const {
    return bytes_.substr(8449);
  }

  std::string whole_file() const {
    return bytes_;
  }

private:
  std::string bytes_;
};

//

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

TEST_F(Serializer, RosStringPrefixedWithLength) {
  RosIoClass<std::string> ros_s{"something"};

  ss << ros_s;

  ASSERT_THAT(ss.str(), Eq(std::string("\x09\0\0\0something", 9+4)));
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

TEST_F(Writer, Bag_Header_Record_Padded_To_Magic_Number_Of_Bytes) {
  Header h = sample.get_header_obj();

  write_bag_header_record(ss, h);

  ASSERT_THAT(ss.str(), Eq(sample.bag_header_record()));
}

TEST_F(Writer, Writes_Message_Definition) {
  MessageDefinition m = sample.get_message_definition_obj();

  write_message_definition(ss, m);

  ASSERT_THAT(ss.str(), Eq(sample.message_definition()));
}

TEST_F(Writer, Writes_TF_Messages) {
  std::vector<Header> hs = sample.get_tf_message_header_objects();
  std::vector<RosMsgTransformStamped> ms = sample.get_tf_message_objects();

  write_messages(ss, hs.begin(), hs.end(), ms.begin());

  ASSERT_THAT(ss.str(), Eq(sample.tf_messages()));
}

TEST_F(Writer, Writes_Connection_Record) {
  Header h = sample.get_connection_header_obj();
  ConnectionHeader ch = sample.get_connection_obj();

  write_connection_record(ss, h, ch);

  ASSERT_THAT(ss.str(), Eq(sample.connection_record()));
}

TEST_F(Writer, Writes_Index_Record) {
  Header h = sample.get_index_header_obj();
  std::vector<IndexEntry> entries = sample.get_index_objects();

  write_index_record(ss, h, entries.begin(), entries.end());

  ASSERT_THAT(ss.str(), Eq(sample.index_record()));
}

TEST_F(Writer, Writes_Chunk_Info_Record) {
  Header h = sample.get_chunk_info_header_obj();
  std::vector<ChunkInfoEntry> entries = sample.get_chunk_info_objects();

  write_chunk_info_record(ss, h, entries.begin(), entries.end());

  ASSERT_THAT(ss.str(), Eq(sample.chunk_info_record()));
}

TEST_F(Writer, Writes_The_Complete_Document_Back) {
  Header h_head  = sample.get_header_obj();
  Header h_chunk = sample.get_chunk_header_obj();
  Header h_conn  = sample.get_connection_header_obj();
  ConnectionHeader ch = sample.get_connection_obj();
  std::vector<Header> hs_message = sample.get_tf_message_header_objects();
  std::vector<RosMsgTransformStamped> ms = sample.get_tf_message_objects();
  Header h_index_data = sample.get_index_header_obj();
  std::vector<IndexEntry> index_entries = sample.get_index_objects();
  Header h_chunk_info = sample.get_chunk_info_header_obj();
  std::vector<ChunkInfoEntry> chunk_info_entries = sample.get_chunk_info_objects();

  write_initial_line(ss);
  write_bag_header_record(ss, h_head);
  write_header(ss, h_chunk);
  auto pos = reserve_size_value(ss);
  write_connection_record(ss, h_conn, ch);
  write_messages(ss, hs_message.begin(), hs_message.end(), ms.begin());
  fixate_size_value(ss, pos);
  write_index_record(ss, h_index_data, index_entries.begin(), index_entries.end());
  write_connection_record(ss, h_conn, ch);
  write_chunk_info_record(ss, h_chunk_info, chunk_info_entries.begin(), chunk_info_entries.end());

  ASSERT_THAT(ss.str(), Eq(sample.whole_file()));
}

// refactor
// Index records: offsets are filled in
// Opcodes are filled in

//wr.will_write_events_of_type(that);
//wr.add_event();
//wr.add_event();
//wr.finalize(); (maybe)

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
