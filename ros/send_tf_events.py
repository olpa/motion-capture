import rospy
import tf
import QuatWithEffect

def talker():
  pub = rospy.Publisher('fake_imu', tf.msg.tfMessage, queue_size=10)
  rospy.init_node('fake_imu', anonymous=True)
  rate = rospy.Rate(10) # 10hz
  quat = QuatWithEffect.QuatWithEffect()
  br = tf.TransformBroadcaster()
  while not rospy.is_shutdown():
    br.sendTransform(
      (0, 0, 0),
      quat.next(),
      rospy.Time.now(),
      "imu_sensor1",
      "world"
    )
    rate.sleep()

if __name__ == '__main__':
  try:
    talker()
  except rospy.ROSInterruptException:
    pass
