from sympy import Symbol

# customize our generator to produce symbols with a specific prefix
class PrefixSymbolIter:
  def __init__(self, prefix):
    self.prefix = prefix

  def __iter__(self):
    self.num = 0
    return self

  def __next__(self):
    label = "{}{}".format(self.prefix, self.num)
    self.num += 1
    return Symbol(label)
