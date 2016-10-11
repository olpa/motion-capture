CXXFLAGS=-std=c++11 -ggdb -Wall
LDUNITTESTFLAGS=-lgmock -lgtest

HPPFILES=if_iostream.hpp measurement.hpp
MPUHPPFILES=if_mpu6050.hpp i2c_lib.hpp
OBJFILES=${patsubst %.hpp,%.o,${HPPFILES}}
MPUOBJFILES=${patsubst %.hpp,%.o,${MPUHPPFILES}}

# Tools

all: replay-hex-values mpu

replay-hex-values: main-replay-hex-values.cpp ${OBJFILES} ${HPPFILES}
	g++ -o $@ $< ${CXXFLAGS} ${OBJFILES}

mpu: main-mpu.cpp ${OBJFILES} ${HPPFILES} ${MPUOBJFILES} ${MPUHPPFILES}
	g++ -o $@ $< ${CXXFLAGS} ${OBJFILES} ${MPUOBJFILES}

# Library

%.o: %.cpp ${HPPFILES}
	g++ -o $@ -c ${CXXFLAGS} $<

# Misc

clean:
	rm -f mpu replay-hex-values run-test-* *.o *~

# Testing

test: texec-calibrate texec-measurement texec-if_iostream

texec-%: run-test-%
	./$<

run-%: %.cpp ${OBJFILES} ${HPPFILES}
	g++ -o $@ ${CXXFLAGS} $< ${OBJFILES} ${LDUNITTESTFLAGS}
