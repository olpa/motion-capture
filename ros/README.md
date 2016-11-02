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

## Decoding the structure of the bag-files

I plan to generate `bag`-files self without `ROS` libraries installed.

There is [documentation](http://wiki.ros.org/Bags/Format/2.0) for the `bag` format, but it is not enough. To fully understand the structure, I've written bag-to-xml converter `bag_to_xml.py`, which works for my samples. Usage:

    $ cat file.bag | python bag_to_xml.py >file.xml

The raw data for a message mostly corresponds to the message structure. The two things I don't understand are:

* The field `seq` in `Header` is always 0.
* There is always the value 1 of the type `uint32` before the `Header`.
