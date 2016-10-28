# rviz and ROS

For 3D visualization I decided to use the tool [rviz](http://wiki.ros.org/rviz) from [ROS](http://www.ros.org/) (robotics operating system).

The setup was hard and not obvious for me. These notes help you to get started fast.

## Moving things in rviz

As the first step, I wanted to move something in rviz.

Moving anything is surprisingly a very hard task for a novice who doesn't know about the right way.

The right way is likely to use the transformation frames ([TF](http://wiki.ros.org/tf/Tutorials/Introduction%20to%20tf)). All the objects are connected to framed and ROS tracks the frames.

The test program `send_tf_events.py` uses the frame named `world` as the main fixed frame, and rotates the frame `imu_sensor1`. The tool `rviz` visualizes the rotation.

![rviz setup](rvizsetup.png)

The two things are important.

1. The component `TF` should be added into the interface.
2. The name of the main fixed frame should be set to `world`.

`Rviz` finds the rotation frame `imu_sensor1` somehow self.

## Recording and replaying the motion

`ROS` provides the tool [rosbag](http://wiki.ros.org/ROS/Tutorials/Recording%20and%20playing%20back%20data).

I've made a sample recording in the file `sample_tf_events.bag`.

If you want to play the same motion twice, reset `rviz` (there is a button). The `TF`-events have a timestamp. Without reset, `rviz` ignores the events "from the past".

## TODO

I plan to decode the structure of `bag`-files in order to generate them self without `ROS` libraries installed.