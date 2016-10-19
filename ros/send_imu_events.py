import rospy
import std_msgs.msg
import sensor_msgs.msg
import QuatWithEffect

def set_quat(obj, vals):
  (obj.x, obj.y, obj.z, obj.w) = vals

def set_vector3(obj, vals):
  (obj.x, obj.y, obj.z) = vals

def talker():
  pub = rospy.Publisher('fake_imu', sensor_msgs.msg.Imu, queue_size=10)
  rospy.init_node('fake_imu', anonymous=True)
  rate = rospy.Rate(10) # 10hz
  quat = QuatWithEffect.QuatWithEffect()
  while not rospy.is_shutdown():
    msg = sensor_msgs.msg.Imu()
    set_quat(msg.orientation, quat.next())
    msg.orientation_covariance = [
      1.0, 0.0, 0.0,
      0.0, 1.0, 0.0,
      0.0, 0.0, 1.0 ]
    set_vector3(msg.angular_velocity,    (0, 0, 0))
    set_vector3(msg.linear_acceleration, (0, 0, 0))
    msg.angular_velocity_covariance    = (0, 0, 0, 0, 0, 0, 0, 0, 0)
    msg.linear_acceleration_covariance = (0, 0, 0, 0, 0, 0, 0, 0, 0)
    rospy.loginfo(msg)
    pub.publish(msg)
    rate.sleep()

if __name__ == '__main__':
  try:
    talker()
  except rospy.ROSInterruptException:
    pass
