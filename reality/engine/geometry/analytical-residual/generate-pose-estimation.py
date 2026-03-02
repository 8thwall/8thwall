# Copyright (c) 2021 8th Wall, Inc.
# Original Author: Dat Chu (dat@8thwall.com)
from argparse import ArgumentParser
from string import Template

'''
Compute PoseEstimationAnalytic based on ReprojectionResidual.

You only need to modify compute_residual if you want to change the formulation. This script will
automatically convert your Python formula to C++ ceres residual. It also auto-memoize sub-expressions
that do not change when you compute per point residual. This happens because for each input parameter
(rx, ry, rz, tx, ty, tz), residual computation loops through all the observation match pairs. Many
subparameters do not change its value unless one of rx, ry, rz, tx, ty, tz are changed.

Setup:
 - pip3 install sympy
Run with:
 - python3 reality/engine/geometry/analytical-residual/pose-estimation.py outputFileName.cc
 although you don't really need to run this. You can invoke analytic::PoseEstimation in your cc
 code (omniscope / unittest) and test it out that way.
'''
from sympy import *
from sympy.printing import cxxcode
from rotation import angle_axis_rotate_point, angle_axis_rotate_point_small_angle
from memoizer import PrefixSymbolIter

def generate_residual_code():
  code_lines = []

  NUM_RESIDUALS = 2  # residual in ray space
  TARGET_DIM = 6  # 6 camera parameters
  # Rotation is in angle, axis format (the vector is the axis, the magnitude is the angle in radian)
  rx, ry, rz = symbols("rx ry rz")
  tx, ty, tz = symbols("tx ty tz")
  # x, y, z is point in 3d
  x, y, z = symbols("x_ y_ z_")
  # u, v, w is the target ray in ray space.
  u, v, w = symbols("u_ v_ w_")
  # scale value used for curvy targets.
  scale_ = symbols("scale_")

  # We cache symbols that are being optimized. These symbols change in the inner loop of optimization
  # because we have to compute the cost function for each point. Cache will be thrashed if it
  # contains these symbols
  UNCACHEABLE_SYMBOLS = [x, y, z, u, v, w]

  def print_cse_combo(res, X, prefix='x', toSimplify=False):
    """Return C++ code after performing sub-expression factoring and memoization. Depends on
    UNCACHEABLE_SYMBOLS.

    Parameters
    ----------
    res: residual (sympy expression)
    X: an array of sympy variables
    prefix: the prefix to add to C++ variable naming
    toSimplify: do we perform expression simplification? Might hang forever.
    """
    # Compute Jacobian AFTER residual function has been optimized
    if toSimplify:
      res = simplify(res)
    J = res.jacobian(X)
    if toSimplify:
      J = simplify(J)

    expressions = [res[i] for i in range(NUM_RESIDUALS)] + \
        [J[i] for i in range(NUM_RESIDUALS * TARGET_DIM)]

    sub_expressions, rewritten_expressions = cse(expressions, symbols=PrefixSymbolIter(prefix))

    # Find memoizable expressions
    memoizable = []
    not_memoizable = []
    not_memoizable_symbols = []
    var_name_to_is_memoizable = {}
    for var_name, expr in sub_expressions:
      current_uncacheable_symbols = not_memoizable_symbols + UNCACHEABLE_SYMBOLS
      is_memoizable = not expr.has(*current_uncacheable_symbols)
      var_name_to_is_memoizable[var_name] = is_memoizable
      if is_memoizable:
        memoizable.append((var_name, expr))
      else:
        not_memoizable.append((var_name, expr))
        not_memoizable_symbols.append(var_name)

    if memoizable:
      # Output memoizable expressions
      code_lines.append("// Memoizable expressions")
      code_lines.append("static double {};".format(",".join([str(var_name) for var_name, _ in memoizable])))

      code_lines.append("if (needsParamUpdate) {")
      for var_name, expr in memoizable:
        code_lines.append("{} = {};".format(var_name, cxxcode(expr)))
      code_lines.append("}")

    # Output non-memoizable expressions
    for var_name, expr in not_memoizable:
      code_lines.append("const double {} = {};".format(var_name, cxxcode(expr)))

    # Final computation output: residuals and jacobians
    for i in range(NUM_RESIDUALS):
      code_lines.append("residuals[{}] = {};".format(i, cxxcode(rewritten_expressions[i])))
    code_lines.append("""
  if (jacobians == NULL) {
  return true;
  }
  double *jacobian = jacobians[0];
  if (jacobian == NULL) {
  return true;
  }
  """)
    for i in range(NUM_RESIDUALS * TARGET_DIM):
      code_lines.append("jacobian[{}] = {};".format(i, cxxcode(rewritten_expressions[NUM_RESIDUALS+i])))
    code_lines.append("return true;")

  def compute_residual(pt):
    """We rotate and translate the point. Then project it to ray space.
    """
    pt[0] += tx
    pt[1] += ty
    pt[2] += tz
    weight = Piecewise((100*w, pt[2] < 0), (w, True))
    # Previously we dampen these cost with a huber function
    # 1e-10 + delta * sqrt(sqrt(1 + (x / delta) ** 2) - 1)
    # with delta = 0.005
    # This is now done at the LossFunction level
    return Matrix([
      ((scale_ * pt[0] / pt[2] - (scale_ * u)) * weight),
      ((scale_ * pt[1] / pt[2] - (scale_ * v)) * weight),
      ])

  # variables that are we optimizing
  X = [rx, ry, rz, tx, ty, tz]

  # Actually printing our C++ code
  code_lines.append("if (theta2 < 1e-6) {")
  res = compute_residual(angle_axis_rotate_point_small_angle([rx, ry, rz], [x, y, z]))
  # NOTE(dat): Automatic simplification doesn't work. I haven't interactively tried to manually
  # pick diffrent simplification
  print_cse_combo(res, X, prefix='x', toSimplify=False)
  code_lines.append("} else {")
  res = compute_residual(angle_axis_rotate_point([rx, ry, rz], [x, y, z]))
  print_cse_combo(res, X, prefix='y', toSimplify=False)
  code_lines.append("}")
  return "\n".join(code_lines)


template = Template('''
#include "reality/engine/geometry/analytical-residual/pose-estimation.h"

namespace c8 {
namespace {
constexpr double SMALL_FLOAT = 1e-12;
double computeObservedPointWeight(const ObservedPoint &pt) {
  // Larger scales weighted less
  const auto scaleWeight = 1.0f / (1.0f + pt.scale / 2.0f);

  // larger descriptor distances weighted less
  const auto distWeight =
  pt.descriptorDist > 1000.0f ? 1.0f : 1.0f / (1.0f + pt.descriptorDist / 100.0f);

  return scaleWeight * distWeight * (pt.weight > SMALL_FLOAT ? pt.weight : 1.0f);
}
}  // namespace

namespace analytic {

PoseEstimation::PoseEstimation(
  double x, double y, double z, const ObservedPoint &pt, double scale)
  : x_(x), y_(y), z_(z), u_(pt.position.x()), v_(pt.position.y()), scale_(scale) {
  w_ = computeObservedPointWeight(pt);
}

bool PoseEstimation::Evaluate(
  double const *const *c, double *residuals, double **jacobians) const {
  static bool first = true;
  static double rx, ry, rz, tx, ty, tz, theta2;
  const int needsParamUpdate = first + (c[0][0] != rx) + (c[0][1] != ry) + (c[0][2] != rz)
  + (c[0][3] != tx) + (c[0][4] != ty) + (c[0][5] != tz);

  first = false;
  rx = c[0][0];
  ry = c[0][1];
  rz = c[0][2];
  tx = c[0][3];
  ty = c[0][4];
  tz = c[0][5];
  theta2 = rx * rx + ry * ry + rz * rz;

  $code
}

} // namespace analytic
} // namespace c8
''')

if __name__ == '__main__':
  parser = ArgumentParser()
  parser.add_argument('output', help='file to write out to')
  args = parser.parse_args()

  residual_code = generate_residual_code()
  file_content = template.substitute(code=residual_code)
  with open(args.output, 'wt') as f:
    f.write(file_content)
