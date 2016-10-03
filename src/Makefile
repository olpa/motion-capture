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
	rm -f mpu replay-hex-values run-test-* ${OBJFILES}

#test: test-calibrate
#	./$<

test: texec-calibrate texec-measurement

texec-calibrate: run-test-calibrate
	./$<

run-test-calibrate: test-calibrate.cpp
	g++ -o $@ ${CXXFLAGS} $< ${LDUNITTESTFLAGS}

texec-measurement: run-test-measurement
	./$<

run-test-measurement: test-measurement.cpp
	g++ -o $@ ${CXXFLAGS} $< ${LDUNITTESTFLAGS}
