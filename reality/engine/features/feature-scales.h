// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <cfloat>
#include <cstdint>

namespace c8 {

static constexpr uint8_t NUM_SCALES = 4;
static constexpr float SCALE_FACTOR = 1.44;

// These constants must be changed if num scales are changed above. The python code to regenerate
// them is given:
/*
  NUM_SCALES = 8;
  SCALE_FACTOR = 1.2;

  print 'static constexpr float MAX_MIN_SCALE_RATIO = %r;'% (SCALE_FACTOR ** NUM_SCALES)
  print 'static constexpr float DMIN_SCALE_LUT[NUM_SCALES] = { ',
  for s in xrange(NUM_SCALES-1, -1, -1):
    print '%r, ' % (SCALE_FACTOR ** -(s + .5)),
  print '};'
  print 'static constexpr float SCALE_FOR_DIST_LUT[NUM_SCALES + 1] = { ',
  for s in xrange(-NUM_SCALES, 1):
    print '%r, ' % (SCALE_FACTOR ** -(s)),
  print '};'
*/
static constexpr float MAX_MIN_SCALE_RATIO = 4.2998169599999985;

static constexpr float DMIN_SCALE_LUT[NUM_SCALES] = {
  0.2790816472336534, 0.40187757201646096, 0.5787037037037037, 0.8333333333333334
  // 0.2547655226259521, 0.3057186271511425, 0.366862352581371, 0.44023482309764517,
  // 0.5282817877171742, 0.633938145260609, 0.7607257743127308, 0.9128709291752769,
};

static constexpr float PATCH_SCALE[NUM_SCALES] = {1.0f, 1.44f, 2.0736, 2.985984f};

static constexpr float INV_PATCH_SCALE[NUM_SCALES] = {
  1.0f / PATCH_SCALE[0],  // 1.0
  1.0f / PATCH_SCALE[1],  // 0.69444444444
  1.0f / PATCH_SCALE[2],  // 0.48225308642
  1.0f / PATCH_SCALE[3],  // 0.33489797668
};

static constexpr float INV_PATCH_SCALE_SQ[NUM_SCALES] = {
  INV_PATCH_SCALE[0] * INV_PATCH_SCALE[0],  // 1.0
  INV_PATCH_SCALE[1] * INV_PATCH_SCALE[1],  // 0.48225308642
  INV_PATCH_SCALE[2] * INV_PATCH_SCALE[2],  // 0.23256803936
  INV_PATCH_SCALE[3] * INV_PATCH_SCALE[3],  // 0.11215665478
};

static constexpr float SCALE_FOR_DIST_LUT[NUM_SCALES + 1] = {
  4.299816959999999, 2.9859839999999997, 2.0736, 1.44, 1.0
  // 4.2998169599999985, 3.583180799999999, 2.9859839999999993, 2.4883199999999994, 2.0736, 1.7279999999999998,
  // 1.44, 1.2, 1.0,
};

constexpr float dMinForScale(uint8_t scale, float dist) { return dist * DMIN_SCALE_LUT[scale]; }

constexpr float dMaxForDMin(float dmin) { return dmin > 0 ? dmin * MAX_MIN_SCALE_RATIO : FLT_MAX; }

constexpr uint8_t scaleForDist(float dmin, float dist) {
  float r = dist / dmin;
  // return log(r)/log(1.2)
  // Scale 0 itentionally omitted.
  return 255 * ((r <= SCALE_FOR_DIST_LUT[NUM_SCALES]) | (r > SCALE_FOR_DIST_LUT[0]))
    // + 0 * ((r > SCALE_FOR_DIST_LUT[1]) & (r <= SCALE_FOR_DIST_LUT[0]))
    + 1 * ((r > SCALE_FOR_DIST_LUT[2]) & (r <= SCALE_FOR_DIST_LUT[1]))
    + 2 * ((r > SCALE_FOR_DIST_LUT[3]) & (r <= SCALE_FOR_DIST_LUT[2]))
    + 3 * ((r > SCALE_FOR_DIST_LUT[4]) & (r <= SCALE_FOR_DIST_LUT[3]))
    //+ 4 * ((r > SCALE_FOR_DIST_LUT[5]) & (r <= SCALE_FOR_DIST_LUT[4]))
    //+ 5 * ((r > SCALE_FOR_DIST_LUT[6]) & (r <= SCALE_FOR_DIST_LUT[5]))
    //+ 6 * ((r > SCALE_FOR_DIST_LUT[7]) & (r <= SCALE_FOR_DIST_LUT[6]))
    //+ 7 * ((r > SCALE_FOR_DIST_LUT[8]) & (r <= SCALE_FOR_DIST_LUT[7]))
    ;
}

}  // namespace c8
