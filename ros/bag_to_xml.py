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
    val = self.xml_escape(val)
    self.h.write("%s<%s>%s</%s>\n" % ("  " * self.level, key, val, key))

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
          code = 0xe000
        if code < 0x7f:
          a.append(ch)
        elif code < 0x100:
          a.extend("&#x%02x;" % code)
        else:
          a.extend("&#x%04x;" % code)
    return "".join(a)

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
        loop_over_header(h, wr, header_len)
      s_data_len = h.read(4)
      data_len = struct.unpack_from("<I", s_data_len)[0]
      s_data = h.read(data_len)

def loop_over_header(h, wr, bytes_left):
  while bytes_left > 0:
    s_field_len = h.read(4)
    field_len = struct.unpack_from("<I", s_field_len)[0]
    s_field = h.read(field_len)
    (field_name, field_value) = s_field.split("=", 2)
    bytes_left = bytes_left - field_len - 4
    wr.keyval(field_name, field_value)
  assert not bytes_left

read_initial_line(sys.stdin)
wr = XmlWriter(sys.stdout)
with wr.in_tag("rosbag"):
  loop_over_records(sys.stdin, wr)
