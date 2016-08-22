CXXFLAGS=-std=c++11

all: replay

replay: replay-hex-values

replay-hex-values: replay-hex-values.cpp
	g++ -o $@ ${CXXFLAGS} $+

mpu: mpu.cpp measurement.o
	g++ -o $@ ${CXXFLAGS} $+
