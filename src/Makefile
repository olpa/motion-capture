CXXFLAGS=-std=c++11 -ggdb
LDUNITTESTFLAGS=-lgmock -lgtest

OBJFILES=measurement.o

all: replay

replay: replay-hex-values

r: replay-hex-values
	./$< got-from-device

replay-hex-values: replay-hex-values.cpp ${OBJFILES}
	g++ -o $@ ${CXXFLAGS} $+

mpu: mpu.cpp ${OBJFILES}
	g++ -o $@ ${CXXFLAGS} $+

clean:
	rm -f mpu replay-hex-values ${OBJFILES}

test: test-calibrate
	./$<

test-calibrate: test-calibrate.cpp
	g++ -o $@ ${CXXFLAGS} $< ${LDUNITTESTFLAGS}
