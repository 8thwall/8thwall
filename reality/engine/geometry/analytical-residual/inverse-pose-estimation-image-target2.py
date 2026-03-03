# Copyright (c) 2021 8th Wall, Inc.
# Original Author: Paris Morgan (paris@8thwall.com)
'''
Compute InversePoseEstimationImageTarget C++ code which you can then copy and paste into bundle-residual-analytical
Setup:
 - pip3 install sympy
Run with:
 - python3 reality/engine/geometry/analytical-residual/inverse-pose-estimation-image-target2.py
'''
from sympy import *
from sympy.printing import cxxcode
from rotation import angle_axis_to_rotation_matrix, angle_axis_to_rotation_matrix_small_angle

def main():
    NUM_RESIDUALS = 2  # x, y of the residual in ray space
    TARGET_DIM = 6    # 6 camera parameters
    # Target is the camera parameter
    # Rotation is in angle, axis format (the vector is the axis, the magnitude is the angle in radian)
    rx, ry, rz = symbols("rx ry rz")
    tx, ty, tz = symbols("tx ty tz")
    # x, y is the camera ray in ray space.
    camRayX, camRayY = symbols("x_ y_")
    # u, v, w is the target ray in ray space.
    u, v, w = symbols("u_ v_ w_")

    def compute_residual(u, v, w, x, y, iz):
        return Matrix([
            w * (u - x * iz),
            w * (v - y * iz),
        ])

    def print_cse_combo(u, v, w, x, y, iz, X, toSimplify=False):
        res = compute_residual(u, v, w, x, y, iz)
        J = res.jacobian(X)
        if toSimplify:
            res = simplify(res)
            J = simplify(J)
        expressions = [res[i] for i in range(NUM_RESIDUALS)] + \
          [J[i] for i in range(NUM_RESIDUALS * TARGET_DIM)]
        sub_expressions, rewritten_expressions = cse(expressions)
        for var_name, expr in sub_expressions:
            print("const double {} = {};".format(var_name, cxxcode(expr)))
        for i in range(NUM_RESIDUALS):
            print("residuals[{}] = {};".format(i, cxxcode(rewritten_expressions[i])))
        print("""
    if (jacobians == NULL) {
      return true;
    }
    double *jacobian = jacobians[0];
    if (jacobian == NULL) {
      return true;
    }
    """)
        for i in range(NUM_RESIDUALS * TARGET_DIM):
            print("jacobian[{}] = {};".format(i, cxxcode(rewritten_expressions[NUM_RESIDUALS+i])))

    X = [rx, ry, rz, tx, ty, tz]

    # Actually printing our C++ code
    print("const double theta2 = rx*rx + ry*ry + rz*rz;")
    print("if (theta2 < 1e-6) {")
    H = angle_axis_to_rotation_matrix_small_angle([rx, ry, rz])
    H[2] += tx
    H[5] += ty
    H[8] += tz
    m = H.inv()
    x = m[0] * camRayX + m[1] * camRayY + m[2]
    y = m[3] * camRayX + m[4] * camRayY + m[5]
    iz = 1 / (m[6] * camRayX + m[7] * camRayY + m[8])
    print_cse_combo(u, v, w, x, y, iz, X, toSimplify=True)
    print("} else {")
    H = angle_axis_to_rotation_matrix([rx, ry, rz])
    H[2] += tx
    H[5] += ty
    H[8] += tz
    m = H.inv()
    x = m[0] * camRayX + m[1] * camRayY + m[2]
    y = m[3] * camRayX + m[4] * camRayY + m[5]
    iz = 1 / (m[6] * camRayX + m[7] * camRayY + m[8])
    print_cse_combo(u, v, w, x, y, iz, X, toSimplify=True)
    print("}")


if __name__ == '__main__':
    main()
