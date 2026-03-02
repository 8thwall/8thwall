# Copyright (c) 2021 8th Wall, Inc.
# Original Author: Dat Chu (dat@8thwall.com)
'''
Compute PositionTargetAnalytical C++ code which you can then copy and paste into bundle-residual-analytical

Setup:
 - pip3 install sympy
Run with:
 - python3 reality/engine/geometry/analytical-residual/position-target.py
'''
from sympy import *
from sympy.printing import cxxcode
from rotation import angle_axis_rotate_point, angle_axis_rotate_point_small_angle

def main():
  # Rotation is in angle, axis format (the vector is the axis, the magnitude is the angle in radian)
  rx, ry, rz = symbols("rx ry rz")
  tx, ty, tz = symbols("tx ty tz")

  # vx, vy, vz is our target position value
  vx, vy, vz = symbols("vx vy vz")

  def compute_residual(cameraInWorld):
    return Matrix([
      cameraInWorld[0] + vx,
      cameraInWorld[1] + vy,
      cameraInWorld[2] + vz,
    ])

  def print_residual(cameraInWorld):
    res = compute_residual(cameraInWorld)

    for i in range(3):
      print("residuals[{}] =".format(i), cxxcode(simplify(res[i])), ";")

  def print_jacobian(cameraInWorld, X):
    J = simplify(compute_residual(cameraInWorld).jacobian(X))

    for i in range(3 * 6):
      print("jacobian[{}] =".format(i), cxxcode(J[i]), ";")

  def print_cse_combo(cameraInWorld, X):
    res = simplify(compute_residual(cameraInWorld))
    J = simplify(res.jacobian(X))
    sub_expressions, rewritten_expressions = cse([res[i] for i in range(3)] + [J[i] for i in range(3 * 6)])
    for var_name, expr in sub_expressions:
      print("{} =".format(var_name), cxxcode(expr))

    for i in range(3):
      print("residuals[{}] =".format(i), cxxcode(rewritten_expressions[i]), ";")
    for i in range(3 * 6):
      print("jacobian[{}] =".format(i), cxxcode(rewritten_expressions[3+i]), ";")


  X = [rx, ry, rz, tx, ty, tz]
  # Actually printing our C++ code
  print("theta2 = rx*rx + ry*ry + rz*rz")
  print("if (theta2 < 1e-6) {")
  print("VALUE COMPUTATION")
  cameraInWorldSmall = angle_axis_rotate_point_small_angle([-rx, -ry, -rz], [tx, ty, tz])
  print_residual(cameraInWorldSmall)
  print("JACOBIAN COMPUTATION")
  print_jacobian(cameraInWorldSmall, X)

  print("} else {")
  cameraInWorldBig = angle_axis_rotate_point([-rx, -ry, -rz], [tx, ty, tz])
  print_cse_combo(cameraInWorldBig, X)
  print("}")


if __name__ == '__main__':
  main()
