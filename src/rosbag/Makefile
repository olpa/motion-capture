CXXFLAGS=-std=c++11 -ggdb -Wall
LDUNITTESTFLAGS=-lgmock -lgtest

all: a.out
	./$<

a.out: test-rosbag_writer.cpp
	g++ $< ${LDUNITTESTFLAGS}
