CXXFLAGS=-std=c++11 -ggdb -Wall
LDUNITTESTFLAGS=-lgmock -lgtest

OBJFILES=*.o
HPPFILES=*.hpp

all: replay

replay: replay-hex-values

r: replay-hex-values
	./$< got-from-device

replay-hex-values: replay-hex-values.cpp ${OBJFILES} ${HPPFILES}
	g++ -o $@ ${CXXFLAGS} $+

mpu: mpu.cpp ${OBJFILES} ${HPPFILES}
	g++ -o $@ ${CXXFLAGS} $+

if_mpu6050.o: if_mpu6050.cpp ${OBJFILES} ${HPPFILES}
	g++ -o $@ ${CXXFLAGS} $+

clean:
	rm -f mpu replay-hex-values run-test-* ${OBJFILES} *~

#test: test-calibrate
#	./$<

test: texec-calibrate texec-measurement texec-iostream

texec-calibrate: run-test-calibrate
	./$<

run-test-calibrate: test-calibrate.cpp ${OBJFILES} ${HPPFILES}
	g++ -o $@ ${CXXFLAGS} $< ${OBJFILES} ${LDUNITTESTFLAGS}

texec-measurement: run-test-measurement
	./$<

run-test-measurement: test-measurement.cpp ${OBJFILES} ${HPPFILES}
	g++ -o $@ ${CXXFLAGS} $< ${OBJFILES} ${LDUNITTESTFLAGS}

texec-iostream: run-test-iostream
	./$<

run-test-iostream: test-if_iostream.cpp ${OBJFILES} ${HPPFILES}
	g++ -o $@ ${CXXFLAGS} $< ${OBJFILES} ${LDUNITTESTFLAGS}
