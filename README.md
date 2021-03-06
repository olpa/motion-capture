# Experiments with the MPU-6050 sensor

The first milestone as the target:

* The code gets data from the sensor, and
* broadcasts the values to `ROS`.
* The tool `rviz` (or maybe also `gazebo`) replays the motion.

https://www.invensense.com/products/motion-tracking/6-axis/mpu-6050/
<br />
http://www.ros.org/
<br />
http://wiki.ros.org/rviz
<br />
http://gazebosim.org/

## The status as of October 2016

The tool `mpu` reads the values from the sensor and dumps the values to the standard output.

The tool `replay-hex-values` reads the values stored in the file. To be used for development when the sensor is detached.

Now I'm understanding that the values from the sensor mean.

On my system, the sensor starts in the sleep mode and should be activated by the command

    $ i2cset -y 1 0x68 0x6b 0

## Files

`doc/`: `MPU-6050` and `I2C` specifications

`ros/`: experiments with `ROS`, `Python`

`sample_measurements/`: the values for basic movements

`src/`: the source code, `C++`
