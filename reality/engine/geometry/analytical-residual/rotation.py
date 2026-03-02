from sympy import *

def angle_axis_rotate_point_small_angle(angle_axis, pt):
  """Rotate a point (x,y,z) by angle axis (rx, ry, rz)
  From ceres-solver/rotation.h
  """
  w_cross_pt = [
    angle_axis[1] * pt[2] - angle_axis[2] * pt[1],
    angle_axis[2] * pt[0] - angle_axis[0] * pt[2],
    angle_axis[0] * pt[1] - angle_axis[1] * pt[0]
  ]

  return [
    pt[0] + w_cross_pt[0],
    pt[1] + w_cross_pt[1],
    pt[2] + w_cross_pt[2]
  ]

def angle_axis_rotate_point(angle_axis, pt):
  theta2 = angle_axis[0]*angle_axis[0] + angle_axis[1]*angle_axis[1] + angle_axis[2]*angle_axis[2]
  theta = sqrt(theta2)
  costheta = cos(theta)
  sintheta = sin(theta)
  theta_inverse = theta**-1

  w = [
    angle_axis[0] * theta_inverse,
    angle_axis[1] * theta_inverse,
    angle_axis[2] * theta_inverse
  ]

  # Explicitly inlined evaluation of the cross product for
  # performance reasons.
  w_cross_pt = [
    w[1] * pt[2] - w[2] * pt[1],
    w[2] * pt[0] - w[0] * pt[2],
    w[0] * pt[1] - w[1] * pt[0]
  ]

  tmp = (w[0] * pt[0] + w[1] * pt[1] + w[2] * pt[2]) * (Float(1.0) - costheta)

  return [
    pt[0] * costheta + w_cross_pt[0] * sintheta + w[0] * tmp,
    pt[1] * costheta + w_cross_pt[1] * sintheta + w[1] * tmp,
    pt[2] * costheta + w_cross_pt[2] * sintheta + w[2] * tmp
  ]

# https://kornia.readthedocs.io/en/v0.1.2/_modules/torchgeometry/core/conversions.html#angle_axis_to_rotation_matrix
# Compare against this
# https://ceres-solver.googlesource.com/ceres-solver/+/master/include/ceres/rotation.h#386
def angle_axis_to_rotation_matrix_small_angle(angle_axis):
    """
    Convert between an angle-axis rotation representation and a 3x3 rotation matrix
    We perform a transpose since ceres' matrix is column-major
    From ceres-solver/rotation.h
    """
    # Near zero, we switch to using the first order Taylor expansion.
    return Matrix(
        [
            [1, angle_axis[2], -angle_axis[1]],
            [-angle_axis[2], 1, angle_axis[0]],
            [angle_axis[1], -angle_axis[0], 1]
        ]
    ).T

def angle_axis_to_rotation_matrix(angle_axis):
    """
    Convert between an angle-axis rotation representation and a 3x3 rotation matrix
    (in column major order - TODO(understand RowMajorAdapter3x3))
    From ceres-solver/rotation.h
    """
    # We want to be careful to only evaluate the square root if the
    # norm of the angle_axis vector is greater than zero. Otherwise
    # we get a division by zero.
    theta2 = angle_axis[0]*angle_axis[0] + angle_axis[1]*angle_axis[1] + angle_axis[2]*angle_axis[2]
    theta = sqrt(theta2)
    wx = angle_axis[0] / theta
    wy = angle_axis[1] / theta
    wz = angle_axis[2] / theta
    costheta = cos(theta)
    sintheta = sin(theta)
    return Matrix(
        [
            [costheta + wx*wx*(1 - costheta), wz*sintheta + wx*wy*(1 - costheta), -wy*sintheta + wx*wz*(1 - costheta)],
            [wx*wy*(1 - costheta) - wz*sintheta, costheta + wy*wy*(1 - costheta), wx*sintheta + wy*wz*(1 - costheta)],
            [wy*sintheta + wx*wz*(1 - costheta), -wx*sintheta + wy*wz*(1 - costheta), costheta + wz*wz*(1 - costheta)]
        ]
    ).T
