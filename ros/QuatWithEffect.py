import geometry_msgs.msg

class QuatWithEffect:
  def __init__(self):
    self.pos = [0, 0, 0, 0]
    self.v = [0.05, 0.10, 0.15, 0.20]

  def __iter__(self):
    return self

  def next(self):
    def new_val(old_val, delta):
      val = old_val + delta
      if val > 1:
        val = -val
      return val
    self.pos = [new_val(*v) for v in zip(self.pos, self.v)]
    return self.pos

if '__main__' == __name__:
  qobj = QuatWithEffect()
  for dummy in xrange(20):
    print qobj.next()
