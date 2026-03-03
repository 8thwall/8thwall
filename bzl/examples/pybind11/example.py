import sys
from os.path import dirname
sys.path.append(dirname(__file__))

#
import example_native

if __name__ == "__main__":
    print(example_native.mult(3, 4))
