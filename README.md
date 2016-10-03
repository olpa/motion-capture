# Experiments with the MPU-6050 sensor

The first milestone as the target:

* The code gets data from the sensor, and
* broadcasts the values to `ROS`.
* The tool `rviz` (or maybe also `gazebo`) replays the motion.

https://www.invensense.com/products/motion-tracking/6-axis/mpu-6050/
http://www.ros.org/
http://wiki.ros.org/rviz
http://gazebosim.org/

## The status as of September 2016

The tool `mpu` reads the values from the sensor and dumps the values to the standard output.

The tool `replay-hex-values` reads the values stored in the file. To be used for development when the sensor is detached.

Now I'm understanding that the values from the sensor mean.

## Files

`doc/`: MPU-6050 and I2C specifications

`sample_measurements/`: the values for basic movements
