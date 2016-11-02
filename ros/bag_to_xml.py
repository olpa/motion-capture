import sys
if ("-h" in sys.argv) or ("--help" in sys.argv):
  print "The minimal bag-file dumper. http://wiki.ros.org/Bags/Format/2.0"
  print "cat file.bag | python bag_to_xml.py >file.xml"
  sys.exit(0)

import struct

class XmlWriter:
  def __init__(self, h):
    self.h = h
    self.level = 0

  def in_tag(self, tname):
    class InTag:
      def __enter__(selfself):
        self.h.write("%s<%s>\n" % ("  " * self.level, tname))
        self.level = self.level + 1
      def __exit__(selfself, type, value, traceback):
        self.level = self.level - 1
        self.h.write("%s</%s>\n" % ("  " * self.level, tname))
    return InTag()

  def keyval(self, key, val):
    attr = ""
    if 4 == len(val):
      attr = " int=\"%i\"" % coded_string_to_int(val)
    elif 8 == len(val):
      attr = " long=\"%i\"" % coded_string_to_long(val)
    val = self.xml_escape(val)
    self.h.write("%s<%s%s>%s</%s>\n" % ("  " * self.level, key, attr, val, key))

  def xml_escape(self, s):
    a = []
    for ch in s:
      if '<' == ch:
        a.extend("&lt;")
      elif '>' == ch:
        a.extend("&gt;")
      elif '&' == ch:
        a.extend("&amp;")
      else:
        code = ord(ch)
        if code < 0x20:
          code = 0xe000 + code
        if code < 0x7f:
          a.append(ch)
        elif code < 0x100:
          a.extend("&#x%02x;" % code)
        else:
          a.extend("&#x%04x;" % code)
    return "".join(a)

def coded_string_to_byte(s):
  return ord(s)

def coded_string_to_int(s):
  return struct.unpack_from("<I", s)[0]

def coded_string_to_long(s):
  return struct.unpack_from("<I", s)[0]

def read_initial_line(h):
  s = h.readline()
  s = s.strip()
  assert "#ROSBAG V2.0" == s

def loop_over_records(h, wr):
  while True:
    s_header_len = h.read(4)
    if not s_header_len:
      break
    with wr.in_tag("record"):
      header_len = struct.unpack_from("<I", s_header_len)[0]
      with wr.in_tag("header"):
        rec = loop_over_header(h, wr, header_len)
      opcode = rec["op"]
      opcode = coded_string_to_byte(opcode)
      s_data_len = h.read(4)
      data_len = struct.unpack_from("<I", s_data_len)[0]
      if 4 == opcode:
        handle_record_index(h, wr, rec, data_len)
      elif 7 == opcode:
        handle_record_connection(h, wr, data_len)
      elif 6 == opcode:
        handle_record_chunk_info(h, wr, rec)
      elif 5 == opcode:
        handle_record_chunk(h, wr, data_len)
      else:
        handle_data_skip(h, wr, data_len)

def handle_record_connection(h, wr, data_len):
  loop_over_header(h, wr, data_len)

def handle_record_chunk(h, wr, data_len):
  loop_over_records(h, wr)

def handle_record_chunk_info(h, rw, rec):
  for i in range(coded_string_to_int(rec["count"])):
    with wr.in_tag("chunk"):
      s = h.read(8)
      wr.keyval("conn", s[:4])
      wr.keyval("count", s[4:])

def handle_record_index(h, rw, rec, data_len):
  for i in range(coded_string_to_int(rec["count"])):
    with wr.in_tag("index"):
      s = h.read(12)
      data_len = data_len - 12
      wr.keyval("time", s[:8])
      wr.keyval("offset", s[8:])
  assert not data_len

def handle_data_skip(h, wr, data_len):
  s_data = h.read(data_len)
  wr.keyval("rawdata", s_data)

def loop_over_header(h, wr, bytes_left):
  rec = {}
  while bytes_left > 0:
    s_field_len = h.read(4)
    field_len = struct.unpack_from("<I", s_field_len)[0]
    s_field = h.read(field_len)
    (field_name, field_value) = s_field.split("=", 1)
    bytes_left = bytes_left - field_len - 4
    wr.keyval(field_name, field_value)
    rec[field_name] = field_value
  assert not bytes_left
  return rec

read_initial_line(sys.stdin)
wr = XmlWriter(sys.stdout)
with wr.in_tag("rosbag"):
  loop_over_records(sys.stdin, wr)
