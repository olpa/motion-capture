import geometry_msgs.msg

class QuatWithEffect:
  def __init__(self):
    self.pos = [0, 0, 0, 0]
    self.v = [0.05, 0.10, 0.15, 0.20]

  def __iter__(self):
    return self

  def next(self):
    def new_val(old_val, delta):
      new_val = old_val + delta
      if (new_val < -1) or (new_val > 1):
        return (old_val, -delta)
      else:
        return (new_val, delta)
    vals = [new_val(*x) for x in zip(self.pos, self.v)]
    (self.pos, self.v) = zip(*vals)
    return self.pos

if '__main__' == __name__:
  qobj = QuatWithEffect()
  for dummy in xrange(20):
    print qobj.next()
