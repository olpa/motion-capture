CXXFLAGS=-std=c++11

all: mpu

mpu: mpu.cpp measurement.o
	g++ -o $@ ${CXXFLAGS} $+
