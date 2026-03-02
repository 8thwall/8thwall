// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

// This file doesn't use inliner because it has two library versions. One with forced NEON
// used during testing.

#include "c8/pixels/pixel-transforms.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>

#include "c8/color-maps.h"
#include "c8/exceptions.h"
#include "c8/pixels/packed.h"
#include "c8/stats/scope-timer.h"
#include "c8/task-queue.h"

#ifdef __ARM_NEON__
#include <arm_neon.h>
#elif FORCE_NEON_CODEPATH
#include <NEON_2_SSE.h>
#endif

namespace c8 {

namespace {

// clang-format off

// Computes the red contribution of the v channel, with a built-in offset to the clip table.
// (i - 128) * 1.4 + 225
static const int16_t rvLut[256]{
  46,  48,  49,  50,  52,  53,  55,  56,  57,  59,  60,  62,  63,  64,  66,  67,  69,  70,  71,
  73,  74,  76,  77,  78,  80,  81,  83,  84,  85,  87,  88,  90,  91,  92,  94,  95,  97,  98,
  100, 101, 102, 104, 105, 107, 108, 109, 111, 112, 113, 115, 116, 118, 119, 120, 122, 123, 125,
  126, 127, 129, 130, 132, 133, 134, 136, 137, 139, 140, 141, 143, 144, 146, 147, 148, 150, 151,
  153, 154, 155, 157, 158, 160, 161, 163, 164, 165, 167, 168, 169, 171, 172, 174, 175, 176, 178,
  179, 181, 182, 183, 185, 186, 188, 189, 190, 192, 193, 195, 196, 197, 199, 200, 202, 203, 204,
  206, 207, 209, 210, 211, 213, 214, 216, 217, 218, 220, 221, 223, 224, 225, 226, 227, 229, 230,
  232, 233, 234, 236, 237, 239, 240, 241, 243, 244, 246, 247, 248, 250, 251, 253, 254, 255, 257,
  258, 260, 261, 262, 264, 265, 267, 268, 269, 271, 272, 274, 275, 276, 278, 279, 281, 282, 283,
  285, 286, 287, 289, 290, 292, 293, 295, 296, 297, 299, 300, 302, 303, 304, 306, 307, 309, 310,
  311, 313, 314, 316, 317, 318, 320, 321, 323, 324, 325, 327, 328, 330, 331, 332, 334, 335, 337,
  338, 339, 341, 342, 343, 345, 346, 348, 349, 350, 352, 353, 355, 356, 358, 359, 360, 362, 363,
  365, 366, 367, 369, 370, 372, 373, 374, 376, 377, 379, 380, 381, 383, 384, 386, 387, 388, 390,
  391, 393, 394, 395, 397, 398, 400, 401, 402};

// Computes the green contribution of the u channel, with a built-in offset to the clip table.
// (i - 128) * -0.343 + 225
static const int16_t guLut[256]{
  268, 268, 268, 267, 267, 267, 266, 266, 266, 265, 265, 265, 264, 264, 264, 263, 263, 263, 262,
  262, 262, 261, 261, 261, 260, 260, 259, 259, 259, 258, 258, 258, 257, 257, 257, 256, 256, 256,
  255, 255, 255, 254, 254, 254, 253, 253, 253, 252, 252, 252, 251, 251, 251, 250, 250, 250, 249,
  249, 249, 248, 248, 247, 247, 247, 246, 246, 246, 245, 245, 245, 244, 244, 244, 243, 243, 243,
  242, 242, 242, 241, 241, 241, 240, 240, 240, 239, 239, 239, 238, 238, 238, 237, 237, 237, 236,
  236, 235, 235, 235, 234, 234, 234, 233, 233, 233, 232, 232, 232, 231, 231, 231, 230, 230, 230,
  229, 229, 229, 228, 228, 228, 227, 227, 227, 226, 226, 226, 225, 225, 225, 225, 225, 224, 224,
  224, 223, 223, 223, 222, 222, 222, 221, 221, 221, 220, 220, 220, 219, 219, 219, 218, 218, 218,
  217, 217, 217, 216, 216, 216, 215, 215, 215, 214, 214, 213, 213, 213, 212, 212, 212, 211, 211,
  211, 210, 210, 210, 209, 209, 209, 208, 208, 208, 207, 207, 207, 206, 206, 206, 205, 205, 205,
  204, 204, 204, 203, 203, 203, 202, 202, 201, 201, 201, 200, 200, 200, 199, 199, 199, 198, 198,
  198, 197, 197, 197, 196, 196, 196, 195, 195, 195, 194, 194, 194, 193, 193, 193, 192, 192, 192,
  191, 191, 191, 190, 190, 189, 189, 189, 188, 188, 188, 187, 187, 187, 186, 186, 186, 185, 185,
  185, 184, 184, 184, 183, 183, 183, 182, 182};

// Computes the green contribution of the v channel. This one doesn't have + 225 because it's added
// to guLut which already has the clip table offset built in..
// (i - 128) * -0.711.
static const int16_t gvLut[256]{
  91,  90,  89,  88,  88,  87,  86,  86,  85,  84,  83,  83,  82,  81,  81,  80,  79,  78,  78,
  77,  76,  76,  75,  74,  73,  73,  72,  71,  71,  70,  69,  68,  68,  67,  66,  66,  65,  64,
  63,  63,  62,  61,  61,  60,  59,  59,  58,  57,  56,  56,  55,  54,  54,  53,  52,  51,  51,
  50,  49,  49,  48,  47,  46,  46,  45,  44,  44,  43,  42,  41,  41,  40,  39,  39,  38,  37,
  36,  36,  35,  34,  34,  33,  32,  31,  31,  30,  29,  29,  28,  27,  27,  26,  25,  24,  24,
  23,  22,  22,  21,  20,  19,  19,  18,  17,  17,  16,  15,  14,  14,  13,  12,  12,  11,  10,
  9,   9,   8,   7,   7,   6,   5,   4,   4,   3,   2,   2,   1,   0,   0,   0,   -1,  -2,  -2,
  -3,  -4,  -4,  -5,  -6,  -7,  -7,  -8,  -9,  -9,  -10, -11, -12, -12, -13, -14, -14, -15, -16,
  -17, -17, -18, -19, -19, -20, -21, -22, -22, -23, -24, -24, -25, -26, -27, -27, -28, -29, -29,
  -30, -31, -31, -32, -33, -34, -34, -35, -36, -36, -37, -38, -39, -39, -40, -41, -41, -42, -43,
  -44, -44, -45, -46, -46, -47, -48, -49, -49, -50, -51, -51, -52, -53, -54, -54, -55, -56, -56,
  -57, -58, -59, -59, -60, -61, -61, -62, -63, -63, -64, -65, -66, -66, -67, -68, -68, -69, -70,
  -71, -71, -72, -73, -73, -74, -75, -76, -76, -77, -78, -78, -79, -80, -81, -81, -82, -83, -83,
  -84, -85, -86, -86, -87, -88, -88, -89, -90};

// Computes the blue contribution of the u channel, with a built-in offset to the clip table.
// (i-128) * 1.765 + 225
static const int16_t buLut[256]{
  0,   1,   3,   5,   7,   8,   10,  12,  14,  15,  17,  19,  21,  23,  24,  26,  28,  30,  31,
  33,  35,  37,  38,  40,  42,  44,  45,  47,  49,  51,  53,  54,  56,  58,  60,  61,  63,  65,
  67,  68,  70,  72,  74,  75,  77,  79,  81,  83,  84,  86,  88,  90,  91,  93,  95,  97,  98,
  100, 102, 104, 105, 107, 109, 111, 113, 114, 116, 118, 120, 121, 123, 125, 127, 128, 130, 132,
  134, 135, 137, 139, 141, 143, 144, 146, 148, 150, 151, 153, 155, 157, 158, 160, 162, 164, 165,
  167, 169, 171, 173, 174, 176, 178, 180, 181, 183, 185, 187, 188, 190, 192, 194, 195, 197, 199,
  201, 203, 204, 206, 208, 210, 211, 213, 215, 217, 218, 220, 222, 224, 225, 226, 228, 230, 232,
  233, 235, 237, 239, 240, 242, 244, 246, 247, 249, 251, 253, 255, 256, 258, 260, 262, 263, 265,
  267, 269, 270, 272, 274, 276, 277, 279, 281, 283, 285, 286, 288, 290, 292, 293, 295, 297, 299,
  300, 302, 304, 306, 307, 309, 311, 313, 315, 316, 318, 320, 322, 323, 325, 327, 329, 330, 332,
  334, 336, 337, 339, 341, 343, 345, 346, 348, 350, 352, 353, 355, 357, 359, 360, 362, 364, 366,
  367, 369, 371, 373, 375, 376, 378, 380, 382, 383, 385, 387, 389, 390, 392, 394, 396, 397, 399,
  401, 403, 405, 406, 408, 410, 412, 413, 415, 417, 419, 420, 422, 424, 426, 427, 429, 431, 433,
  435, 436, 438, 440, 442, 443, 445, 447, 449};

// A look-up table for implimenting "clip" without branching. Based on the limits of yuv, we need
// to handle values in the range [-225, 255+224] inclusive.
static const uint8_t uvClip[705]{
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   2,
  3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,
  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,
  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,
  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116,
  117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
  136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154,
  155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173,
  174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192,
  193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211,
  212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
  231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249,
  250, 251, 252, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255};

static const uint8_t yr[256]{
  0,  0,  1,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,
  7,  7,  8,  8,  8,  9,  9,  9,  10, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14,
  14, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 21, 21, 21,
  22, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 28,
  29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36,
  36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 42, 43,
  43, 43, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 50, 50,
  50, 51, 51, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 57,
  57, 58, 58, 58, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 64, 64, 64,
  65, 65, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 71, 71,
  72, 72, 72, 73, 73, 73, 74, 74, 74, 74, 75, 75, 75, 76, 76, 76};

static const uint8_t yg[256]{
  0,   1,   1,   2,   2,   3,   4,   4,   5,   5,   6,   6,   7,   8,   8,   9,   9,   10,  11,
  11,  12,  12,  13,  14,  14,  15,  15,  16,  16,  17,  18,  18,  19,  19,  20,  21,  21,  22,
  22,  23,  23,  24,  25,  25,  26,  26,  27,  28,  28,  29,  29,  30,  31,  31,  32,  32,  33,
  33,  34,  35,  35,  36,  36,  37,  38,  38,  39,  39,  40,  41,  41,  42,  42,  43,  43,  44,
  45,  45,  46,  46,  47,  48,  48,  49,  49,  50,  50,  51,  52,  52,  53,  53,  54,  55,  55,
  56,  56,  57,  58,  58,  59,  59,  60,  60,  61,  62,  62,  63,  63,  64,  65,  65,  66,  66,
  67,  68,  68,  69,  69,  70,  70,  71,  72,  72,  73,  73,  74,  75,  75,  76,  76,  77,  77,
  78,  79,  79,  80,  80,  81,  82,  82,  83,  83,  84,  85,  85,  86,  86,  87,  87,  88,  89,
  89,  90,  90,  91,  92,  92,  93,  93,  94,  95,  95,  96,  96,  97,  97,  98,  99,  99,  100,
  100, 101, 102, 102, 103, 103, 104, 104, 105, 106, 106, 107, 107, 108, 109, 109, 110, 110, 111,
  112, 112, 113, 113, 114, 114, 115, 116, 116, 117, 117, 118, 119, 119, 120, 120, 121, 122, 122,
  123, 123, 124, 124, 125, 126, 126, 127, 127, 128, 129, 129, 130, 130, 131, 131, 132, 133, 133,
  134, 134, 135, 136, 136, 137, 137, 138, 139, 139, 140, 140, 141, 141, 142, 143, 143, 144, 144,
  145, 146, 146, 147, 147, 148, 149, 149, 150};

static const uint8_t yb[256]{
  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,
  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,
  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,
  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11,
  11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14,
  14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16,
  16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19,
  19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22,
  22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 25,
  25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27,
  27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29};

// lookup table for converting uint8_t [0, 255] to float [0, 1]
static const float f0to1[256]{
  0.00000000, 0.00392157, 0.00784314, 0.01176471, 0.01568628, 0.01960784, 0.02352941, 0.02745098,
  0.03137255, 0.03529412, 0.03921569, 0.04313726, 0.04705882, 0.05098039, 0.05490196, 0.05882353,
  0.06274510, 0.06666667, 0.07058824, 0.07450981, 0.07843138, 0.08235294, 0.08627451, 0.09019608,
  0.09411765, 0.09803922, 0.10196079, 0.10588235, 0.10980392, 0.11372549, 0.11764706, 0.12156863,
  0.12549020, 0.12941177, 0.13333334, 0.13725491, 0.14117648, 0.14509805, 0.14901961, 0.15294118,
  0.15686275, 0.16078432, 0.16470589, 0.16862746, 0.17254902, 0.17647059, 0.18039216, 0.18431373,
  0.18823530, 0.19215687, 0.19607843, 0.20000000, 0.20392157, 0.20784314, 0.21176471, 0.21568628,
  0.21960784, 0.22352941, 0.22745098, 0.23137255, 0.23529412, 0.23921569, 0.24313726, 0.24705882,
  0.25098041, 0.25490198, 0.25882354, 0.26274511, 0.26666668, 0.27058825, 0.27450982, 0.27843139,
  0.28235295, 0.28627452, 0.29019609, 0.29411766, 0.29803923, 0.30196080, 0.30588236, 0.30980393,
  0.31372550, 0.31764707, 0.32156864, 0.32549021, 0.32941177, 0.33333334, 0.33725491, 0.34117648,
  0.34509805, 0.34901962, 0.35294119, 0.35686275, 0.36078432, 0.36470589, 0.36862746, 0.37254903,
  0.37647060, 0.38039216, 0.38431373, 0.38823530, 0.39215687, 0.39607844, 0.40000001, 0.40392157,
  0.40784314, 0.41176471, 0.41568628, 0.41960785, 0.42352942, 0.42745098, 0.43137255, 0.43529412,
  0.43921569, 0.44313726, 0.44705883, 0.45098040, 0.45490196, 0.45882353, 0.46274510, 0.46666667,
  0.47058824, 0.47450981, 0.47843137, 0.48235294, 0.48627451, 0.49019608, 0.49411765, 0.49803922,
  0.50196081, 0.50588238, 0.50980395, 0.51372552, 0.51764709, 0.52156866, 0.52549022, 0.52941179,
  0.53333336, 0.53725493, 0.54117650, 0.54509807, 0.54901963, 0.55294120, 0.55686277, 0.56078434,
  0.56470591, 0.56862748, 0.57254905, 0.57647061, 0.58039218, 0.58431375, 0.58823532, 0.59215689,
  0.59607846, 0.60000002, 0.60392159, 0.60784316, 0.61176473, 0.61568630, 0.61960787, 0.62352943,
  0.62745100, 0.63137257, 0.63529414, 0.63921571, 0.64313728, 0.64705884, 0.65098041, 0.65490198,
  0.65882355, 0.66274512, 0.66666669, 0.67058825, 0.67450982, 0.67843139, 0.68235296, 0.68627453,
  0.69019610, 0.69411767, 0.69803923, 0.70196080, 0.70588237, 0.70980394, 0.71372551, 0.71764708,
  0.72156864, 0.72549021, 0.72941178, 0.73333335, 0.73725492, 0.74117649, 0.74509805, 0.74901962,
  0.75294119, 0.75686276, 0.76078433, 0.76470590, 0.76862746, 0.77254903, 0.77647060, 0.78039217,
  0.78431374, 0.78823531, 0.79215688, 0.79607844, 0.80000001, 0.80392158, 0.80784315, 0.81176472,
  0.81568629, 0.81960785, 0.82352942, 0.82745099, 0.83137256, 0.83529413, 0.83921570, 0.84313726,
  0.84705883, 0.85098040, 0.85490197, 0.85882354, 0.86274511, 0.86666667, 0.87058824, 0.87450981,
  0.87843138, 0.88235295, 0.88627452, 0.89019608, 0.89411765, 0.89803922, 0.90196079, 0.90588236,
  0.90980393, 0.91372550, 0.91764706, 0.92156863, 0.92549020, 0.92941177, 0.93333334, 0.93725491,
  0.94117647, 0.94509804, 0.94901961, 0.95294118, 0.95686275, 0.96078432, 0.96470588, 0.96862745,
  0.97254902, 0.97647059, 0.98039216, 0.98431373, 0.98823529, 0.99215686, 0.99607843, 1.00000000};

// lookup table for converting uint8_t [0, 255] to float [-1, 1]
static const float fn1to1[256]{
  -1.00000000, -0.99215686, -0.98431373, -0.97647059, -0.96862745, -0.96078432, -0.95294118, -0.94509804,
  -0.93725491, -0.92941177, -0.92156863, -0.91372550, -0.90588236, -0.89803922, -0.89019608, -0.88235295,
  -0.87450981, -0.86666667, -0.85882354, -0.85098040, -0.84313726, -0.83529413, -0.82745099, -0.81960785,
  -0.81176472, -0.80392158, -0.79607844, -0.78823531, -0.78039217, -0.77254903, -0.76470590, -0.75686276,
  -0.74901962, -0.74117649, -0.73333335, -0.72549021, -0.71764708, -0.70980394, -0.70196080, -0.69411767,
  -0.68627453, -0.67843139, -0.67058825, -0.66274512, -0.65490198, -0.64705884, -0.63921571, -0.63137257,
  -0.62352943, -0.61568630, -0.60784316, -0.60000002, -0.59215689, -0.58431375, -0.57647061, -0.56862748,
  -0.56078434, -0.55294120, -0.54509807, -0.53725493, -0.52941179, -0.52156866, -0.51372552, -0.50588238,
  -0.49803919, -0.49019605, -0.48235291, -0.47450978, -0.46666664, -0.45882350, -0.45098037, -0.44313723,
  -0.43529409, -0.42745095, -0.41960782, -0.41176468, -0.40392154, -0.39607841, -0.38823527, -0.38039213,
  -0.37254900, -0.36470586, -0.35686272, -0.34901959, -0.34117645, -0.33333331, -0.32549018, -0.31764704,
  -0.30980390, -0.30196077, -0.29411763, -0.28627449, -0.27843136, -0.27058822, -0.26274508, -0.25490195,
  -0.24705881, -0.23921567, -0.23137254, -0.22352940, -0.21568626, -0.20784312, -0.19999999, -0.19215685,
  -0.18431371, -0.17647058, -0.16862744, -0.16078430, -0.15294117, -0.14509803, -0.13725489, -0.12941176,
  -0.12156862, -0.11372548, -0.10588235, -0.09803921, -0.09019607, -0.08235294, -0.07450980, -0.06666666,
  -0.05882353, -0.05098039, -0.04313725, -0.03529412, -0.02745098, -0.01960784, -0.01176471, -0.00392157,
  0.00392163, 0.01176476, 0.01960790, 0.02745104, 0.03529418, 0.04313731, 0.05098045, 0.05882359,
  0.06666672, 0.07450986, 0.08235300, 0.09019613, 0.09803927, 0.10588241, 0.11372554, 0.12156868,
  0.12941182, 0.13725495, 0.14509809, 0.15294123, 0.16078436, 0.16862750, 0.17647064, 0.18431377,
  0.19215691, 0.20000005, 0.20784318, 0.21568632, 0.22352946, 0.23137259, 0.23921573, 0.24705887,
  0.25490201, 0.26274514, 0.27058828, 0.27843142, 0.28627455, 0.29411769, 0.30196083, 0.30980396,
  0.31764710, 0.32549024, 0.33333337, 0.34117651, 0.34901965, 0.35686278, 0.36470592, 0.37254906,
  0.38039219, 0.38823533, 0.39607847, 0.40392160, 0.41176474, 0.41960788, 0.42745101, 0.43529415,
  0.44313729, 0.45098042, 0.45882356, 0.46666670, 0.47450984, 0.48235297, 0.49019611, 0.49803925,
  0.50588238, 0.51372552, 0.52156866, 0.52941179, 0.53725493, 0.54509807, 0.55294120, 0.56078434,
  0.56862748, 0.57647061, 0.58431375, 0.59215689, 0.60000002, 0.60784316, 0.61568630, 0.62352943,
  0.63137257, 0.63921571, 0.64705884, 0.65490198, 0.66274512, 0.67058825, 0.67843139, 0.68627453,
  0.69411767, 0.70196080, 0.70980394, 0.71764708, 0.72549021, 0.73333335, 0.74117649, 0.74901962,
  0.75686276, 0.76470590, 0.77254903, 0.78039217, 0.78823531, 0.79607844, 0.80392158, 0.81176472,
  0.81960785, 0.82745099, 0.83529413, 0.84313726, 0.85098040, 0.85882354, 0.86666667, 0.87450981,
  0.88235295, 0.89019608, 0.89803922, 0.90588236, 0.91372550, 0.92156863, 0.92941177, 0.93725491,
  0.94509804, 0.95294118, 0.96078432, 0.96862745, 0.97647059, 0.98431373, 0.99215686, 1.00000000};

// clang-format on

HMatrix rgbToYCbCrAnalogRaw(float kr, float kg, float kb) {
  return {
    {kr, kg, kb, 0.0f},
    {-0.5f * kr / (1.0f - kb), -0.5f * kg / (1.0f - kb), 0.5f, 0.0f},
    {0.5f, -0.5f * kg / (1.0f - kr), -0.5f * kb / (1.0f - kr), 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 2.0f - 2.0f * kr, 0.0f},
    {1.0f, -(kb / kg) * (2.0f - 2.0f * kb), -(kr / kg) * (2.0f - 2.0f * kr), 0.0f},
    {1.0f, 2.0f - 2.0f * kb, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}};
}

 inline uint8_t clampToByte(float value) {
    return static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, value)));
}

}  // namespace

namespace ColorMat {

HMatrix rgbToYCbCrAnalog(float kr, float kg, float kb) {
  return HMatrixGen::translation(0.0f, 128.0f, 128.0f)
    * rgbToYCbCrAnalogRaw(kr, kg, kb);
}

HMatrix rgbToYCbCrDigital(float kr, float kg, float kb) {
  return HMatrixGen::translation(16.0f, 128.0f, 128.0f)
    * HMatrixGen::scale(219.0f / 255.0f, 224.0f / 255.0f, 224.0f / 255.0f)
    * rgbToYCbCrAnalogRaw(kr, kg, kb);
}

HMatrix rgbToYCbCrBt601Digital() { return rgbToYCbCrDigital(0.299f, 0.587f, 0.114f); }
HMatrix rgbToYCbCrBt709Digital() { return rgbToYCbCrDigital(0.2126f, 0.7152f, 0.0722f); }
HMatrix rgbToYCbCrBt2020NcDigital() { return rgbToYCbCrDigital(0.2627f, 0.6780f, 0.0593f); }
HMatrix rgbToYCbCrJpg() { return rgbToYCbCrAnalog(0.299f, 0.587f, 0.114f); }

}  // namespace ColorMat

void copyPixels(const ConstPixels &src, Pixels *dest) {
  ScopeTimer t("copy-pixels");
  size_t rowCopy = std::min(src.rowBytes(), dest->rowBytes());
  for (int r = 0; r < src.rows(); ++r) {
    std::memcpy(dest->pixels() + r * dest->rowBytes(), src.pixels() + r * src.rowBytes(), rowCopy);
  }
}

void copyFloatPixels(const ConstFloatPixels &src, FloatPixels *dest) {
  ScopeTimer t("copy-float-pixels");
  size_t rowCopy = std::min(src.rowElements(), dest->rowElements());
  for (int r = 0; r < src.rows(); ++r) {
    std::copy(
      src.pixels() + r * src.rowElements(),
      src.pixels() + r * src.rowElements() + rowCopy,
      dest->pixels() + r * dest->rowElements());
  }
}

void downsize(const ConstOneChannelPixels &s, OneChannelPixels *d) {
  if (s.rows() == d->rows() && s.cols() == d->cols() && s.rowBytes() == d->rowBytes()) {
    std::memcpy(d->pixels(), s.pixels(), s.rows() * s.rowBytes());
    return;
  }

  size_t skip = s.rows() / d->rows();
  if (skip * d->rows() != s.rows() || skip * d->cols() != s.cols()) {
    C8_THROW("Downsize only supports integer resize factors.");
  }

  // start at a pixel that is roughly in the center of the sampling region.
  const uint8_t *srcstart = s.pixels() + skip / 2;
  uint8_t *deststart = d->pixels();
  uint8_t *destend = deststart + d->cols();

  size_t destrowskip = d->rowBytes();
  size_t srcrowskip = skip * s.rowBytes();

  for (int i = 0; i < d->rows(); ++i) {
    const uint8_t *srcpix = srcstart;
    uint8_t *destpix = deststart;
    while (destpix != destend) {
      destpix[0] = srcpix[0];
      destpix += 1;
      srcpix += skip;
    }
    srcstart += srcrowskip;
    deststart += destrowskip;
    destend += destrowskip;
  }
}

void downsize(const ConstTwoChannelPixels &s, TwoChannelPixels *d) {
  if (s.rows() == d->rows() && s.cols() == d->cols() && s.rowBytes() == d->rowBytes()) {
    std::memcpy(d->pixels(), s.pixels(), s.rows() * s.rowBytes());
    return;
  }

  size_t skip = s.rows() / d->rows();
  if (skip * d->rows() != s.rows() || skip * d->cols() != s.cols()) {
    C8_THROW("Downsize only supports integer resize factors.");
  }

  // start at a pixel that is roughly in the center of the sampling region.
  size_t skip2 = skip * 2;
  const uint8_t *srcstart = s.pixels() + (skip / 2) * 2;
  uint8_t *deststart = d->pixels();
  uint8_t *destend = deststart + 2 * d->cols();

  size_t destrowskip = d->rowBytes();
  size_t srcrowskip = skip * s.rowBytes();

  for (int i = 0; i < d->rows(); ++i) {
    const uint8_t *srcpix = srcstart;
    uint8_t *destpix = deststart;
    while (destpix != destend) {
      destpix[0] = srcpix[0];
      destpix[1] = srcpix[1];
      destpix += 2;
      srcpix += skip2;
    }
    srcstart += srcrowskip;
    deststart += destrowskip;
    destend += destrowskip;
  }
}

/*
 * Downsizing for four channel RGBA
 */
void downsize(const ConstFourChannelPixels &s, FourChannelPixels *d) {
  if (s.rows() == d->rows() && s.cols() == d->cols() && s.rowBytes() == d->rowBytes()) {
    std::memcpy(d->pixels(), s.pixels(), s.rows() * s.rowBytes());
    return;
  }

  size_t skip = s.rows() / d->rows();

  if (skip * d->rows() != s.rows() || skip * d->cols() != s.cols()) {
    C8_THROW("Downsize only supports integer resize factors.");
  }

  // start at a pixel that is roughly in the center of the sampling region.
  size_t skip4 = skip * 4;

  const uint8_t *srcstart = s.pixels() + (skip / 2) * 4;
  uint8_t *deststart = d->pixels();
  uint8_t *destend = deststart + 4 * d->cols();

  size_t destrowskip = d->rowBytes();
  size_t srcrowskip = skip * s.rowBytes();

  for (int i = 0; i < d->rows(); ++i) {
    const uint8_t *srcpix = srcstart;
    uint8_t *destpix = deststart;
    while (destpix != destend) {
      destpix[0] = srcpix[0];
      destpix[1] = srcpix[1];
      destpix[2] = srcpix[2];
      destpix[3] = srcpix[3];
      destpix += 4;
      srcpix += skip4;
    }
    srcstart += srcrowskip;
    deststart += destrowskip;
    destend += destrowskip;
  }
}

void mergePixels(
  const ConstOneChannelPixels &src1, const ConstOneChannelPixels &src2, TwoChannelPixels *dest) {
  ScopeTimer t("merge-onechannel-pixels");
  if (
    src1.rows() != src2.rows() || src1.rows() != dest->rows() || src1.cols() != src2.cols()
    || src1.cols() != dest->cols()) {
    C8_THROW("src1, src2, dest have to match num rows and num cols");
  }
  for (int r = 0; r < dest->rows(); r++) {
    uint8_t *destBuf = dest->pixels() + r * dest->rowBytes();
    const uint8_t *srcBuf1 = src1.pixels() + r * src1.rowBytes();
    const uint8_t *srcBuf2 = src2.pixels() + r * src2.rowBytes();

    for (int c = 0; c < dest->cols(); c++) {
      destBuf[2 * c] = srcBuf1[c];
      destBuf[2 * c + 1] = srcBuf2[c];
    }
  }
}

// NOTE(dat): This implementation doesn't respect rowBytes
void mergePixels(
  const ConstUSkipPlanePixels &src1, const ConstVSkipPlanePixels &src2, TwoChannelPixels *dest) {
  ScopeTimer t("merge-uv-skipchannel-pixels");

  uint8_t *destBuf = dest->pixels();
  const uint8_t *srcBuf1 = src1.pixels();
  const uint8_t *srcBuf2 = src2.pixels();
  const uint8_t *endDestBuf = destBuf + (dest->rows() * dest->rowBytes());
  const uint8_t *endBuf1 = srcBuf1 + (src1.rows() * src1.rowBytes());
  const uint8_t *endBuf2 = srcBuf2 + (src2.rows() * src2.rowBytes());

  while (destBuf < endDestBuf && (srcBuf1 < endBuf1 || srcBuf2 < endBuf2)) {
    if (srcBuf1 < endBuf1) {
      destBuf[0] = srcBuf1[0];
      srcBuf1 += 2;
    }

    if (srcBuf2 < endBuf2) {
      destBuf[1] = srcBuf2[0];
      srcBuf2 += 2;
    }

    destBuf += 2;
  }
}

void splitPixels(ConstUVPlanePixels src, UPlanePixels dest1, VPlanePixels dest2) {
  if (
    src.rows() != dest1.rows() || src.rows() != dest2.rows() || src.cols() != dest1.cols()
    || src.cols() != dest2.cols()) {
    C8_THROW("src and dest have to match both num rows and num cols");
  }
  const int srcHeight = src.rows();
  const int srcWidth = src.cols();
  const int srcStride = src.rowBytes();
  const uint8_t *srcStart = src.pixels();

  uint8_t *dest1Start = dest1.pixels();
  uint8_t *dest2Start = dest2.pixels();
  const int dest1Stride = dest1.rowBytes();
  const int dest2Stride = dest2.rowBytes();

  for (int r = 0; r < srcHeight; ++r) {
    const uint8_t *s = srcStart + r * srcStride;
    uint8_t *d1 = dest1Start + r * dest1Stride;
    uint8_t *d2 = dest2Start + r * dest2Stride;
    for (int c = 0; c < srcWidth; ++c) {
      d1[c] = s[2 * c];
      d2[c] = s[2 * c + 1];
    }
  }
}

template <int N>
void splitPixels(const ConstFourChannelPixels &src, OneChannelPixels *c[N]) {
  ScopeTimer t("split-four-channels-to-planes");
  int dStride[N];
  uint8_t *d[N];

  for (int i = 0; i < N; ++i) {
    dStride[i] = c[i]->rowBytes();
    d[i] = c[i]->pixels();
  }

  const int srcHeight = src.rows();
  const int srcWidth = src.cols();
  const int srcWidthMultipleOfFour = (srcWidth / 4) * 4;
  const int srcStride = src.rowBytes();
  const uint8_t *srcStart = src.pixels();

  for (int r = 0; r < srcHeight; ++r) {
    const uint8_t *s = srcStart;
    int c = 0;
    for (; c < srcWidthMultipleOfFour; c += 4) {
      const auto blocks = reinterpret_cast<const uint32_t *>(s + (c << 2));
      uint32_t block0 = blocks[0];
      uint32_t block1 = blocks[1];
      uint32_t block2 = blocks[2];
      uint32_t block3 = blocks[3];

      for (int b = 0; b < N; ++b) {
        uint32_t tmp = (block0 & 0xFF);
        tmp |= ((block1 & 0xFF) << 8);
        tmp |= ((block2 & 0xFF) << 16);
        tmp |= ((block3 & 0xFF) << 24);

        auto output = reinterpret_cast<uint32_t *>(d[b] + c);
        *output = tmp;

        block0 = block0 >> 8;
        block1 = block1 >> 8;
        block2 = block2 >> 8;
        block3 = block3 >> 8;
      }
    }
    // Fill in any remaining bytes for non-multiple-by-four images.
    for (; c < srcWidth; ++c) {
      for (int b = 0; b < N; ++b) {
        *(d[b] + c) = *(s + (c << 2) + b);
      }
    }
    for (int i = 0; i < N; ++i) {
      d[i] += dStride[i];
    }
    srcStart += srcStride;
  }
}

void splitPixels(
  const ConstFourChannelPixels &src,
  OneChannelPixels *c1,
  OneChannelPixels *c2,
  OneChannelPixels *c3,
  OneChannelPixels *c4) {

  if (c4 != nullptr) {
    OneChannelPixels *channels[4] = {c1, c2, c3, c4};
    splitPixels<4>(src, channels);
    return;
  }

  if (c3 != nullptr) {
    OneChannelPixels *channels[3] = {c1, c2, c3};
    splitPixels<3>(src, channels);
    return;
  }

  if (c2 != nullptr) {
    OneChannelPixels *channels[2] = {c1, c2};
    splitPixels<2>(src, channels);
    return;
  }

  if (c1 != nullptr) {
    OneChannelPixels *channels[1] = {c1};
    splitPixels<1>(src, channels);
    return;
  }
}

namespace {

void rotate90ClockwiseUnroll4(const ConstOneChannelPixels &src, OneChannelPixels *dest) {
  ScopeTimer t("r90-unroll4-1channel");
  const int srcHeight = src.rows();
  const int srcWidth = src.cols();
  const int srcStride = src.rowBytes();
  const uint8_t *srcBuf = src.pixels();
  const int destStride = dest->rowBytes();
  uint8_t *const destBuf = dest->pixels();

  const uint8_t *srcEnd = srcBuf + srcHeight * srcStride;

  const int srcStride2 = srcStride * 2;
  const int srcStride3 = srcStride * 3;
  const int srcStride4 = srcStride * 4;

  const int destStride2 = destStride * 2;
  const int destStride3 = destStride * 3;
  const int destStride4 = destStride * 4;
  uint8_t *destColStart = destBuf + srcHeight - 4;

  for (const uint8_t *srcRowStart = srcBuf; srcRowStart < srcEnd;
       srcRowStart += srcStride4, destColStart -= 4) {
    const uint8_t *srcRowEnd = srcRowStart + srcWidth;
    const uint8_t *s0 = srcRowStart;
    const uint8_t *s1 = s0 + srcStride;
    const uint8_t *s2 = s0 + srcStride2;
    const uint8_t *s3 = s0 + srcStride3;
    uint8_t *d0 = destColStart;
    uint8_t *d1 = destColStart + destStride;
    uint8_t *d2 = destColStart + destStride2;
    uint8_t *d3 = destColStart + destStride3;

    while (s0 < srcRowEnd) {
      d0[3] = s0[0];
      d0[2] = s1[0];
      d0[1] = s2[0];
      d0[0] = s3[0];
      d1[3] = s0[1];
      d1[2] = s1[1];
      d1[1] = s2[1];
      d1[0] = s3[1];
      d2[3] = s0[2];
      d2[2] = s1[2];
      d2[1] = s2[2];
      d2[0] = s3[2];
      d3[3] = s0[3];
      d3[2] = s1[3];
      d3[1] = s2[3];
      d3[0] = s3[3];

      s0 += 4;
      s1 += 4;
      s2 += 4;
      s3 += 4;
      d0 += destStride4;
      d1 += destStride4;
      d2 += destStride4;
      d3 += destStride4;
    }
  }
}

void rotate90ClockwiseUnroll8(const ConstOneChannelPixels &src, OneChannelPixels *dest) {
  ScopeTimer t("r90-unroll8-1channel");
  const int srcHeight = src.rows();
  const int srcWidth = src.cols();
  const int srcStride = src.rowBytes();
  const uint8_t *srcBuf = src.pixels();
  const int destStride = dest->rowBytes();
  uint8_t *const destBuf = dest->pixels();

  const uint8_t *srcEnd = srcBuf + srcHeight * srcStride;

  const int srcStride2 = srcStride * 2;
  const int srcStride3 = srcStride * 3;
  const int srcStride4 = srcStride * 4;
  const int srcStride5 = srcStride * 5;
  const int srcStride6 = srcStride * 6;
  const int srcStride7 = srcStride * 7;
  const int srcStride8 = srcStride * 8;

  const int destStride2 = destStride * 2;
  const int destStride3 = destStride * 3;
  const int destStride4 = destStride * 4;
  const int destStride5 = destStride * 5;
  const int destStride6 = destStride * 6;
  const int destStride7 = destStride * 7;
  const int destStride8 = destStride * 8;
  uint8_t *destColStart = destBuf + srcHeight - 8;

  for (const uint8_t *srcRowStart = srcBuf; srcRowStart < srcEnd;
       srcRowStart += srcStride8, destColStart -= 8) {
    const uint8_t *srcRowEnd = srcRowStart + srcWidth;
    const uint8_t *s0 = srcRowStart;
    const uint8_t *s1 = s0 + srcStride;
    const uint8_t *s2 = s0 + srcStride2;
    const uint8_t *s3 = s0 + srcStride3;
    const uint8_t *s4 = s0 + srcStride4;
    const uint8_t *s5 = s0 + srcStride5;
    const uint8_t *s6 = s0 + srcStride6;
    const uint8_t *s7 = s0 + srcStride7;
    uint8_t *d0 = destColStart;
    uint8_t *d1 = destColStart + destStride;
    uint8_t *d2 = destColStart + destStride2;
    uint8_t *d3 = destColStart + destStride3;
    uint8_t *d4 = destColStart + destStride4;
    uint8_t *d5 = destColStart + destStride5;
    uint8_t *d6 = destColStart + destStride6;
    uint8_t *d7 = destColStart + destStride7;

    while (s0 < srcRowEnd) {
      d0[7] = s0[0];
      d0[6] = s1[0];
      d0[5] = s2[0];
      d0[4] = s3[0];
      d0[3] = s4[0];
      d0[2] = s5[0];
      d0[1] = s6[0];
      d0[0] = s7[0];
      d1[7] = s0[1];
      d1[6] = s1[1];
      d1[5] = s2[1];
      d1[4] = s3[1];
      d1[3] = s4[1];
      d1[2] = s5[1];
      d1[1] = s6[1];
      d1[0] = s7[1];
      d2[7] = s0[2];
      d2[6] = s1[2];
      d2[5] = s2[2];
      d2[4] = s3[2];
      d2[3] = s4[2];
      d2[2] = s5[2];
      d2[1] = s6[2];
      d2[0] = s7[2];
      d3[7] = s0[3];
      d3[6] = s1[3];
      d3[5] = s2[3];
      d3[4] = s3[3];
      d3[3] = s4[3];
      d3[2] = s5[3];
      d3[1] = s6[3];
      d3[0] = s7[3];
      d4[7] = s0[4];
      d4[6] = s1[4];
      d4[5] = s2[4];
      d4[4] = s3[4];
      d4[3] = s4[4];
      d4[2] = s5[4];
      d4[1] = s6[4];
      d4[0] = s7[4];
      d5[7] = s0[5];
      d5[6] = s1[5];
      d5[5] = s2[5];
      d5[4] = s3[5];
      d5[3] = s4[5];
      d5[2] = s5[5];
      d5[1] = s6[5];
      d5[0] = s7[5];
      d6[7] = s0[6];
      d6[6] = s1[6];
      d6[5] = s2[6];
      d6[4] = s3[6];
      d6[3] = s4[6];
      d6[2] = s5[6];
      d6[1] = s6[6];
      d6[0] = s7[6];
      d7[7] = s0[7];
      d7[6] = s1[7];
      d7[5] = s2[7];
      d7[4] = s3[7];
      d7[3] = s4[7];
      d7[2] = s5[7];
      d7[1] = s6[7];
      d7[0] = s7[7];

      s0 += 8;
      s1 += 8;
      s2 += 8;
      s3 += 8;
      s4 += 8;
      s5 += 8;
      s6 += 8;
      s7 += 8;
      d0 += destStride8;
      d1 += destStride8;
      d2 += destStride8;
      d3 += destStride8;
      d4 += destStride8;
      d5 += destStride8;
      d6 += destStride8;
      d7 += destStride8;
    }
  }
}

void rotate90ClockwiseUnroll4(const ConstTwoChannelPixels &src, TwoChannelPixels *dest) {
  ScopeTimer t("r90-unroll4-2channel");
  const int srcHeight = src.rows();
  const int srcWidth = src.cols();
  const int srcStride = src.rowBytes();
  const uint8_t *srcBuf = src.pixels();
  const int destStride = dest->rowBytes();
  uint8_t *const destBuf = dest->pixels();

  const uint8_t *srcEnd = srcBuf + srcHeight * srcStride;

  const int srcStride2 = srcStride * 2;
  const int srcStride3 = srcStride * 3;
  const int srcStride4 = srcStride * 4;

  const int destStride2 = destStride * 2;
  const int destStride3 = destStride * 3;
  const int destStride4 = destStride * 4;
  uint8_t *destColStart = destBuf + 2 * srcHeight - 8;

  for (const uint8_t *srcRowStart = srcBuf; srcRowStart < srcEnd;
       srcRowStart += srcStride4, destColStart -= 8) {
    const uint8_t *srcRowEnd = srcRowStart + 2 * srcWidth;
    const uint8_t *s0 = srcRowStart;
    const uint8_t *s1 = s0 + srcStride;
    const uint8_t *s2 = s0 + srcStride2;
    const uint8_t *s3 = s0 + srcStride3;
    uint8_t *d0 = destColStart;
    uint8_t *d1 = destColStart + destStride;
    uint8_t *d2 = destColStart + destStride2;
    uint8_t *d3 = destColStart + destStride3;

    while (s0 < srcRowEnd) {
      d0[7] = s0[1];
      d0[6] = s0[0];
      d0[5] = s1[1];
      d0[4] = s1[0];
      d0[3] = s2[1];
      d0[2] = s2[0];
      d0[1] = s3[1];
      d0[0] = s3[0];

      d1[7] = s0[3];
      d1[6] = s0[2];
      d1[5] = s1[3];
      d1[4] = s1[2];
      d1[3] = s2[3];
      d1[2] = s2[2];
      d1[1] = s3[3];
      d1[0] = s3[2];

      d2[7] = s0[5];
      d2[6] = s0[4];
      d2[5] = s1[5];
      d2[4] = s1[4];
      d2[3] = s2[5];
      d2[2] = s2[4];
      d2[1] = s3[5];
      d2[0] = s3[4];

      d3[7] = s0[7];
      d3[6] = s0[6];
      d3[5] = s1[7];
      d3[4] = s1[6];
      d3[3] = s2[7];
      d3[2] = s2[6];
      d3[1] = s3[7];
      d3[0] = s3[6];

      s0 += 8;
      s1 += 8;
      s2 += 8;
      s3 += 8;
      d0 += destStride4;
      d1 += destStride4;
      d2 += destStride4;
      d3 += destStride4;
    }
  }
}

void rotate90ClockwiseUnroll8(const ConstTwoChannelPixels &src, TwoChannelPixels *dest) {
  ScopeTimer t("r90-unroll8-2channel");
  const int srcHeight = src.rows();
  const int srcWidth = src.cols();
  const int srcStride = src.rowBytes();
  const uint8_t *srcBuf = src.pixels();
  const int destStride = dest->rowBytes();
  uint8_t *const destBuf = dest->pixels();

  const uint8_t *srcEnd = srcBuf + srcHeight * srcStride;

  const int srcStride2 = srcStride * 2;
  const int srcStride3 = srcStride * 3;
  const int srcStride4 = srcStride * 4;
  const int srcStride5 = srcStride * 5;
  const int srcStride6 = srcStride * 6;
  const int srcStride7 = srcStride * 7;
  const int srcStride8 = srcStride * 8;

  const int destStride2 = destStride * 2;
  const int destStride3 = destStride * 3;
  const int destStride4 = destStride * 4;
  const int destStride5 = destStride * 5;
  const int destStride6 = destStride * 6;
  const int destStride7 = destStride * 7;
  const int destStride8 = destStride * 8;
  uint8_t *destColStart = destBuf + 2 * srcHeight - 16;

  for (const uint8_t *srcRowStart = srcBuf; srcRowStart < srcEnd;
       srcRowStart += srcStride8, destColStart -= 16) {
    const uint8_t *srcRowEnd = srcRowStart + 2 * srcWidth;
    const uint8_t *s0 = srcRowStart;
    const uint8_t *s1 = s0 + srcStride;
    const uint8_t *s2 = s0 + srcStride2;
    const uint8_t *s3 = s0 + srcStride3;
    const uint8_t *s4 = s0 + srcStride4;
    const uint8_t *s5 = s0 + srcStride5;
    const uint8_t *s6 = s0 + srcStride6;
    const uint8_t *s7 = s0 + srcStride7;
    uint8_t *d0 = destColStart;
    uint8_t *d1 = destColStart + destStride;
    uint8_t *d2 = destColStart + destStride2;
    uint8_t *d3 = destColStart + destStride3;
    uint8_t *d4 = destColStart + destStride4;
    uint8_t *d5 = destColStart + destStride5;
    uint8_t *d6 = destColStart + destStride6;
    uint8_t *d7 = destColStart + destStride7;

    while (s0 < srcRowEnd) {
      d0[15] = s0[1];
      d0[14] = s0[0];
      d0[13] = s1[1];
      d0[12] = s1[0];
      d0[11] = s2[1];
      d0[10] = s2[0];
      d0[9] = s3[1];
      d0[8] = s3[0];
      d0[7] = s4[1];
      d0[6] = s4[0];
      d0[5] = s5[1];
      d0[4] = s5[0];
      d0[3] = s6[1];
      d0[2] = s6[0];
      d0[1] = s7[1];
      d0[0] = s7[0];

      d1[15] = s0[3];
      d1[14] = s0[2];
      d1[13] = s1[3];
      d1[12] = s1[2];
      d1[11] = s2[3];
      d1[10] = s2[2];
      d1[9] = s3[3];
      d1[8] = s3[2];
      d1[7] = s4[3];
      d1[6] = s4[2];
      d1[5] = s5[3];
      d1[4] = s5[2];
      d1[3] = s6[3];
      d1[2] = s6[2];
      d1[1] = s7[3];
      d1[0] = s7[2];

      d2[15] = s0[5];
      d2[14] = s0[4];
      d2[13] = s1[5];
      d2[12] = s1[4];
      d2[11] = s2[5];
      d2[10] = s2[4];
      d2[9] = s3[5];
      d2[8] = s3[4];
      d2[7] = s4[5];
      d2[6] = s4[4];
      d2[5] = s5[5];
      d2[4] = s5[4];
      d2[3] = s6[5];
      d2[2] = s6[4];
      d2[1] = s7[5];
      d2[0] = s7[4];

      d3[15] = s0[7];
      d3[14] = s0[6];
      d3[13] = s1[7];
      d3[12] = s1[6];
      d3[11] = s2[7];
      d3[10] = s2[6];
      d3[9] = s3[7];
      d3[8] = s3[6];
      d3[7] = s4[7];
      d3[6] = s4[6];
      d3[5] = s5[7];
      d3[4] = s5[6];
      d3[3] = s6[7];
      d3[2] = s6[6];
      d3[1] = s7[7];
      d3[0] = s7[6];

      d4[15] = s0[9];
      d4[14] = s0[8];
      d4[13] = s1[9];
      d4[12] = s1[8];
      d4[11] = s2[9];
      d4[10] = s2[8];
      d4[9] = s3[9];
      d4[8] = s3[8];
      d4[7] = s4[9];
      d4[6] = s4[8];
      d4[5] = s5[9];
      d4[4] = s5[8];
      d4[3] = s6[9];
      d4[2] = s6[8];
      d4[1] = s7[9];
      d4[0] = s7[8];

      d5[15] = s0[11];
      d5[14] = s0[10];
      d5[13] = s1[11];
      d5[12] = s1[10];
      d5[11] = s2[11];
      d5[10] = s2[10];
      d5[9] = s3[11];
      d5[8] = s3[10];
      d5[7] = s4[11];
      d5[6] = s4[10];
      d5[5] = s5[11];
      d5[4] = s5[10];
      d5[3] = s6[11];
      d5[2] = s6[10];
      d5[1] = s7[11];
      d5[0] = s7[10];

      d6[15] = s0[13];
      d6[14] = s0[12];
      d6[13] = s1[13];
      d6[12] = s1[12];
      d6[11] = s2[13];
      d6[10] = s2[12];
      d6[9] = s3[13];
      d6[8] = s3[12];
      d6[7] = s4[13];
      d6[6] = s4[12];
      d6[5] = s5[13];
      d6[4] = s5[12];
      d6[3] = s6[13];
      d6[2] = s6[12];
      d6[1] = s7[13];
      d6[0] = s7[12];

      d7[15] = s0[15];
      d7[14] = s0[14];
      d7[13] = s1[15];
      d7[12] = s1[14];
      d7[11] = s2[15];
      d7[10] = s2[14];
      d7[9] = s3[15];
      d7[8] = s3[14];
      d7[7] = s4[15];
      d7[6] = s4[14];
      d7[5] = s5[15];
      d7[4] = s5[14];
      d7[3] = s6[15];
      d7[2] = s6[14];
      d7[1] = s7[15];
      d7[0] = s7[14];

      s0 += 16;
      s1 += 16;
      s2 += 16;
      s3 += 16;
      s4 += 16;
      s5 += 16;
      s6 += 16;
      s7 += 16;
      d0 += destStride8;
      d1 += destStride8;
      d2 += destStride8;
      d3 += destStride8;
      d4 += destStride8;
      d5 += destStride8;
      d6 += destStride8;
      d7 += destStride8;
    }
  }
}

void rotateIntoTwoChannel90ClockWise(
  const int srcHeight,
  const int srcWidth,
  const int srcStride,
  const int srcPixelStride,
  const uint8_t *srcBuf1,
  const uint8_t *srcBuf2,
  TwoChannelPixels *dest) {
  ScopeTimer t("r90-2x1channel-to-2channel");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int destStride = dest->rowBytes();
  uint8_t *const destBuf = dest->pixels();

  // Offset address of the first pixel in a row of the destination.
  //
  // The destination is rotated 90 degrees from the source, so we need to start writing to the
  // destination in the upper right column, moving downward with each pixel, and leftward with
  // each row.
  int srcRowStart = 0;
  int destCol = 2 * (srcHeight - 1);
  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    // Set the current pixels to the start of the current row.
    const uint8_t *srcPix1 = srcBuf1 + srcRowStart;
    const uint8_t *srcPix2 = srcBuf2 + srcRowStart;
    uint8_t *destPix1 = destBuf + destCol;
    uint8_t *destPix2 = destPix1 + 1;

    for (int sourceCol = 0; sourceCol < srcWidth; ++sourceCol) {
      // Copy the pixel.
      *destPix1 = *srcPix1;
      *destPix2 = *srcPix2;

      // Interleaved source columns advance by two.
      srcPix1 += srcPixelStride;
      srcPix2 += srcPixelStride;

      // We need to jump a row in the destination for every column in the source.
      destPix1 += destStride;
      destPix2 += destStride;
    }

    // Move left in the destination.
    destCol -= 2;

    // Interleaved source row advances by uStride (equals vStride).
    srcRowStart += srcStride;
  }
}

void rotateIntoTwoChannel180ClockWise(
  const int srcHeight,
  const int srcWidth,
  const int srcStride,
  const int srcPixelStride,
  const uint8_t *srcBuf1,
  const uint8_t *srcBuf2,
  TwoChannelPixels *dest) {
  ScopeTimer t("r180-2x1channel-to-2channel");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int destStride = dest->rowBytes();
  uint8_t *const destBuf = dest->pixels();

  // Offset address of the first pixel in a row of the destination.
  //
  // The destination is rotated 180 degrees from the source, so we need to start writing to the
  // destination in the bottom right column, moving leftward with each pixel, and upward with
  // each row.
  int srcRowStart = 0;
  int destCol = destStride * (srcHeight - 1) + 2 * (srcWidth - 1);
  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    // Set the current pixels to the start of the current row.
    const uint8_t *srcPix1 = srcBuf1 + srcRowStart;
    const uint8_t *srcPix2 = srcBuf2 + srcRowStart;
    uint8_t *destPix1 = destBuf + destCol;
    uint8_t *destPix2 = destPix1 + 1;

    for (int sourceCol = 0; sourceCol < srcWidth; ++sourceCol) {
      // Copy the pixel.
      *destPix1 = *srcPix1;
      *destPix2 = *srcPix2;

      // Interleaved source columns advance by two.
      srcPix1 += srcPixelStride;
      srcPix2 += srcPixelStride;

      // We need to jump a row in the destination for every column in the source.
      destPix1 -= 2;
      destPix2 -= 2;
    }

    // Move left in the destination.
    destCol -= destStride;

    // Interleaved source row advances by uStride (equals vStride).
    srcRowStart += srcStride;
  }
}

void rotateIntoTwoChannel270ClockWise(
  const int srcHeight,
  const int srcWidth,
  const int srcStride,
  const int srcPixelStride,
  const uint8_t *srcBuf1,
  const uint8_t *srcBuf2,
  TwoChannelPixels *dest) {
  ScopeTimer t("r270-2x1channel-to-2channel");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int destStride = dest->rowBytes();
  uint8_t *const destBuf = dest->pixels();

  // Offset address of the first pixel in a row of the destination.
  //
  // The destination is rotated 270 degrees from the source, so we need to start writing to the
  // destination in the bottom left column, moving upward with each pixel, and rightward with
  // each row.
  int srcRowStart = 0;
  int destCol = (srcWidth - 1) * destStride;
  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    // Set the current pixels to the start of the current row.
    const uint8_t *srcPix1 = srcBuf1 + srcRowStart;
    const uint8_t *srcPix2 = srcBuf2 + srcRowStart;
    uint8_t *destPix1 = destBuf + destCol;
    uint8_t *destPix2 = destPix1 + 1;

    for (int sourceCol = 0; sourceCol < srcWidth; ++sourceCol) {
      // Copy the pixel.
      *destPix1 = *srcPix1;
      *destPix2 = *srcPix2;

      // Interleaved source columns advance by two.
      srcPix1 += srcPixelStride;
      srcPix2 += srcPixelStride;

      // We need to jump a row in the destination for every column in the source.
      destPix1 -= destStride;
      destPix2 -= destStride;
    }

    // Move right in the destination.
    destCol += 2;

    // Interleaved source row advances by uStride (equals vStride).
    srcRowStart += srcStride;
  }
}

void downsizeAndRotate90ClockwiseIntoTwoChannel(
  const int srcHeight,
  const int srcWidth,
  const int srcStride,
  const int srcPixelStride,
  const uint8_t *srcBuf1,
  const uint8_t *srcBuf2,
  TwoChannelPixels *dest) {

  ScopeTimer t("downsize-r90-into-2channel");

  const int destHeight = dest->rows();
  const int destWidth = dest->cols();
  const int destStride = dest->rowBytes();
  uint8_t *const destBuf = dest->pixels();

  const float destWidthScale = srcHeight * 1.0f / destWidth;
  const float destHeightScale = srcWidth * 1.0f / destHeight;
  const float destScale = destWidthScale < destHeightScale ? destWidthScale : destHeightScale;

  const float cropWidthInSrc = destScale * destHeight;
  const float cropHeightInSrc = destScale * destWidth;

  const float srcRowStart = (srcHeight - cropHeightInSrc) / 2;
  const int srcColStart = (srcWidth - cropWidthInSrc) / 2;

  // Offset address of the first pixel in a row of the destination.
  //
  // The destination is rotated 90 degrees from the source, so we need to start writing to the
  // destination in the upper right column, moving downward with each pixel, and leftward with
  // each row.
  int destCol = destWidth - 1;

  for (int i = 0; i < destWidth; ++i) {
    // Set the current pixels to the start of the current row.
    int sourceRow = srcRowStart + i * destScale;

    const uint8_t *srcPix1 = srcBuf1 + sourceRow * srcStride + (srcColStart * srcPixelStride);
    const uint8_t *srcPix2 = srcBuf2 + sourceRow * srcStride + (srcColStart * srcPixelStride);
    uint8_t *destPix = destBuf + destCol + destCol;
    float colOffset = 0;

    for (int j = 0; j < destHeight; ++j) {
      int colIndex = colOffset;
      // Copy the pixel.
      destPix[0] = srcPix1[colIndex * srcPixelStride];
      destPix[1] = srcPix2[colIndex * srcPixelStride];

      colOffset += destScale;

      // We need to jump a row in the destination for every column in the source.
      destPix += destStride;
    }

    // Move left in the destination.
    destCol--;
  }
}

}  // namespace

void rotate90Clockwise(const ConstOneChannelPixels &src, OneChannelPixels *dest) {
  if (src.rows() % 8 == 0 && src.cols() % 8 == 0) {
    rotate90ClockwiseUnroll8(src, dest);
    return;
  }
  if (src.rows() % 4 == 0 && src.cols() % 4 == 0) {
    rotate90ClockwiseUnroll4(src, dest);
    return;
  }
  ScopeTimer t("r90-1channel");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int srcHeight = src.rows();
  const int srcWidth = src.cols();
  const int srcStride = src.rowBytes();
  const uint8_t *srcBuf = src.pixels();
  const int destStride = dest->rowBytes();
  uint8_t *const destBuf = dest->pixels();

  // Offset address of the first pixel in a row of the destination.
  //
  // The destination is rotated 90 degrees from the source, so we need to start writing to the
  // destination in the upper right column, moving downward with each pixel, and leftward with
  // each row.
  int srcRowStart = 0;
  int destCol = srcHeight - 1;

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    // Set the current pixels to the start of the current row.
    const uint8_t *srcPix = srcBuf + srcRowStart;
    uint8_t *destPix = destBuf + destCol;

    for (int sourceCol = 0; sourceCol < srcWidth; ++sourceCol) {
      // Copy the pixel.
      *destPix = *srcPix;

      srcPix++;

      // We need to jump a row in the destination for every column in the source.
      destPix += destStride;
    }

    // Move left in the destination.
    destCol--;

    // Source row advances by yStride every time.
    srcRowStart += srcStride;
  }
}

void rotateFloat90Clockwise(const ConstOneChannelFloatPixels &src, OneChannelFloatPixels *dest) {

  ScopeTimer t("r90-float");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int srcHeight = src.rows();
  const int srcWidth = src.cols();
  const int srcStride = src.rowElements();
  const float *srcBuf = src.pixels();
  const int destStride = dest->rowElements();
  float *const destBuf = dest->pixels();

  // Offset address of the first pixel in a row of the destination.
  //
  // The destination is rotated 90 degrees from the source, so we need to start writing to the
  // destination in the upper right column, moving downward with each pixel, and leftward with
  // each row.
  int srcRowStart = 0;
  int destCol = srcHeight - 1;

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    // Set the current pixels to the start of the current row.
    const float *srcPix = srcBuf + srcRowStart;
    float *destPix = destBuf + destCol;

    for (int sourceCol = 0; sourceCol < srcWidth; ++sourceCol) {
      // Copy the pixel.
      *destPix = *srcPix;

      srcPix++;

      // We need to jump a row in the destination for every column in the source.
      destPix += destStride;
    }

    // Move left in the destination.
    destCol--;

    // Source row advances by yStride every time.
    srcRowStart += srcStride;
  }
}

void rotate180Clockwise(const ConstOneChannelPixels &src, OneChannelPixels *dest) {
  /*
  if (src.rows() % 8 == 0 && src.cols() % 8 == 0) {
    rotate90ClockwiseUnroll8(src, dest);
    return;
  }
  if (src.rows() % 4 == 0 && src.cols() % 4 == 0) {
    rotate90ClockwiseUnroll4(src, dest);
    return;
  }
  */
  ScopeTimer t("r180-1channel");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int srcHeight = src.rows();
  const int srcWidth = src.cols();
  const int srcStride = src.rowBytes();
  const uint8_t *srcBuf = src.pixels();
  const int destStride = dest->rowBytes();
  uint8_t *const destBuf = dest->pixels();

  // Offset address of the first pixel in a row of the destination.
  //
  // The destination is rotated 180 degrees from the source, so we need to start writing to the
  // destination in the lower right column, moving leftward with each pixel, and upward with
  // each row.
  int srcRowStart = 0;
  int destCol = destStride * (srcHeight - 1) + srcWidth - 1;

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    // Set the current pixels to the start of the current row.
    const uint8_t *srcPix = srcBuf + srcRowStart;
    uint8_t *destPix = destBuf + destCol;

    for (int sourceCol = 0; sourceCol < srcWidth; ++sourceCol) {
      // Copy the pixel.
      *destPix = *srcPix;

      srcPix++;

      // We need to jump a row in the destination for every column in the source.
      destPix--;
    }

    // Move left in the destination.
    destCol -= destStride;

    // Source row advances by yStride every time.
    srcRowStart += srcStride;
  }
}

void rotate270Clockwise(const ConstOneChannelPixels &src, OneChannelPixels *dest) {
  /*
  if (src.rows() % 8 == 0 && src.cols() % 8 == 0) {
    rotate90ClockwiseUnroll8(src, dest);
    return;
  }
  if (src.rows() % 4 == 0 && src.cols() % 4 == 0) {
    rotate90ClockwiseUnroll4(src, dest);
    return;
  }
  */
  ScopeTimer t("r270-1channel");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int srcHeight = src.rows();
  const int srcWidth = src.cols();
  const int srcStride = src.rowBytes();
  const uint8_t *srcBuf = src.pixels();
  const int destStride = dest->rowBytes();
  uint8_t *const destBuf = dest->pixels();

  // Offset address of the first pixel in a row of the destination.
  //
  // The destination is rotated 270 degrees from the source, so we need to start writing to the
  // destination in the lower-left column, moving upward with each pixel, and rightward with
  // each row.
  int srcRowStart = 0;
  int destCol = (srcWidth - 1) * destStride;

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    // Set the current pixels to the start of the current row.
    const uint8_t *srcPix = srcBuf + srcRowStart;
    uint8_t *destPix = destBuf + destCol;

    for (int sourceCol = 0; sourceCol < srcWidth; ++sourceCol) {
      // Copy the pixel.
      *destPix = *srcPix;

      srcPix++;

      // We need to jump a row in the destination for every column in the source.
      destPix -= destStride;
    }

    // Move right in the destination.
    destCol++;

    // Source row advances by yStride every time.
    srcRowStart += srcStride;
  }
}

void rotate90Clockwise(const ConstTwoChannelPixels &src, TwoChannelPixels *dest) {
  // TODO(nb): src.rows() % 8 == 0 && src.cols() % 8 == 0
  if (false) {
    rotate90ClockwiseUnroll8(src, dest);
    return;
  }
  // TODO(nb): src.rows() % 4 == 0 && src.cols() % 4 == 0
  if (false) {
    rotate90ClockwiseUnroll4(src, dest);
    return;
  }

  ScopeTimer t("r90-2channel");
  const uint8_t *srcBuf1 = src.pixels();
  const uint8_t *srcBuf2 = srcBuf1 + 1;
  rotateIntoTwoChannel90ClockWise(
    src.rows(), src.cols(), src.rowBytes(), 2, srcBuf1, srcBuf2, dest);
}

void rotate90Clockwise(
  const ConstUSkipPlanePixels &srcU, const ConstVSkipPlanePixels &srcV, TwoChannelPixels *dest) {
  rotateIntoTwoChannel90ClockWise(
    srcU.rows(), srcU.cols(), srcU.rowBytes(), 2, srcU.pixels(), srcV.pixels(), dest);
}

void rotate90Clockwise(
  const ConstUPlanePixels &srcU, const ConstVPlanePixels &srcV, TwoChannelPixels *dest) {
  rotateIntoTwoChannel90ClockWise(
    srcU.rows(), srcU.cols(), srcU.rowBytes(), 1, srcU.pixels(), srcV.pixels(), dest);
}

void rotate180Clockwise(const ConstTwoChannelPixels &src, TwoChannelPixels *dest) {
  // TODO(nb): src.rows() % 8 == 0 && src.cols() % 8 == 0
  /*
  if (false) {
    rotate90ClockwiseUnroll8(src, dest);
    return;
  }
  // TODO(nb): src.rows() % 4 == 0 && src.cols() % 4 == 0
  if (false) {
    rotate90ClockwiseUnroll4(src, dest);
    return;
  }
  */

  ScopeTimer t("r90-2channel");
  const uint8_t *srcBuf1 = src.pixels();
  const uint8_t *srcBuf2 = srcBuf1 + 1;
  rotateIntoTwoChannel180ClockWise(
    src.rows(), src.cols(), src.rowBytes(), 2, srcBuf1, srcBuf2, dest);
}

void rotate180Clockwise(
  const ConstUSkipPlanePixels &srcU, const ConstVSkipPlanePixels &srcV, TwoChannelPixels *dest) {
  rotateIntoTwoChannel180ClockWise(
    srcU.rows(), srcU.cols(), srcU.rowBytes(), 2, srcU.pixels(), srcV.pixels(), dest);
}

void rotate180Clockwise(
  const ConstUPlanePixels &srcU, const ConstVPlanePixels &srcV, TwoChannelPixels *dest) {
  rotateIntoTwoChannel180ClockWise(
    srcU.rows(), srcU.cols(), srcU.rowBytes(), 1, srcU.pixels(), srcV.pixels(), dest);
}

void rotate270Clockwise(const ConstTwoChannelPixels &src, TwoChannelPixels *dest) {
  // TODO(nb): src.rows() % 8 == 0 && src.cols() % 8 == 0
  /*
  if (false) {
    rotate90ClockwiseUnroll8(src, dest);
    return;
  }
  // TODO(nb): src.rows() % 4 == 0 && src.cols() % 4 == 0
  if (false) {
    rotate90ClockwiseUnroll4(src, dest);
    return;
  }
  */

  ScopeTimer t("r90-2channel");
  const uint8_t *srcBuf1 = src.pixels();
  const uint8_t *srcBuf2 = srcBuf1 + 1;
  rotateIntoTwoChannel270ClockWise(
    src.rows(), src.cols(), src.rowBytes(), 2, srcBuf1, srcBuf2, dest);
}

void rotate270Clockwise(
  const ConstUSkipPlanePixels &srcU, const ConstVSkipPlanePixels &srcV, TwoChannelPixels *dest) {
  rotateIntoTwoChannel270ClockWise(
    srcU.rows(), srcU.cols(), srcU.rowBytes(), 2, srcU.pixels(), srcV.pixels(), dest);
}

void rotate270Clockwise(
  const ConstUPlanePixels &srcU, const ConstVPlanePixels &srcV, TwoChannelPixels *dest) {
  rotateIntoTwoChannel270ClockWise(
    srcU.rows(), srcU.cols(), srcU.rowBytes(), 1, srcU.pixels(), srcV.pixels(), dest);
}

void rotate90Clockwise(const ConstFourChannelPixels &src, FourChannelPixels *dest) {
  ScopeTimer t("r90-4channel");
  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int srcHeight = src.rows();
  const int srcWidth = src.cols();
  const int srcStride = src.rowBytes();
  const uint8_t *srcBuf = src.pixels();
  const int destStride = dest->rowBytes();
  uint8_t *const destBuf = dest->pixels();

  // Offset address of the first pixel in a row of the destination.
  //
  // The destination is rotated 90 degrees from the source, so we need to start writing to the
  // destination in the upper right column, moving downward with each pixel, and leftward with
  // each row.
  int srcRowStart = 0;
  int destCol = 4 * (srcHeight - 1);

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    // Set the current pixels to the start of the current row.
    const uint8_t *srcPix = srcBuf + srcRowStart;
    uint8_t *destPix = destBuf + destCol;

    for (int sourceCol = 0; sourceCol < srcWidth; ++sourceCol) {
      // Copy the pixel.
      destPix[0] = srcPix[0];
      destPix[1] = srcPix[1];
      destPix[2] = srcPix[2];
      destPix[3] = srcPix[3];

      srcPix += 4;

      // We need to jump a row in the destination for every column in the source.
      destPix += destStride;
    }

    // Move left in the destination.
    destCol -= 4;

    // Source row advances by yStride every time.
    srcRowStart += srcStride;
  }
}

void rotate270Clockwise(const ConstFourChannelPixels &src, FourChannelPixels *dest) {
  ScopeTimer t("r90-4channel");
  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int srcHeight = src.rows();
  const int srcWidth = src.cols();
  const int srcStride = src.rowBytes();
  const uint8_t *srcBuf = src.pixels();
  const int destStride = dest->rowBytes();
  uint8_t *const destBuf = dest->pixels();

  // Offset address of the first pixel in a row of the destination.
  //
  // The destination is rotated 270 degrees from the source, so we need to start writing to the
  // destination in the lower-left column, moving upward with each pixel, and rightward with
  // each row.
  int srcRowStart = 0;
  int destCol = (srcWidth - 1) * destStride;

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    // Set the current pixels to the start of the current row.
    const uint8_t *srcPix = srcBuf + srcRowStart;
    uint8_t *destPix = destBuf + destCol;

    for (int sourceCol = 0; sourceCol < srcWidth; ++sourceCol) {
      // Copy the pixel.
      destPix[0] = srcPix[0];
      destPix[1] = srcPix[1];
      destPix[2] = srcPix[2];
      destPix[3] = srcPix[3];

      srcPix += 4;

      // We need to jump a row in the destination for every column in the source.
      destPix -= destStride;
    }

    // Move left in the destination.
    destCol += 4;

    // Source row advances by yStride every time.
    srcRowStart += srcStride;
  }
}

void downsizeAndRotate90Clockwise(const ConstOneChannelPixels &src, OneChannelPixels *dest) {
  if (src.rows() == dest->cols() && src.cols() == dest->rows()) {
    rotate90Clockwise(src, dest);
    return;
  }
  ScopeTimer t("downsize-r90-1channel");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int srcHeight = src.rows();
  const int srcWidth = src.cols();
  const int srcStride = src.rowBytes();
  const uint8_t *srcBuf = src.pixels();

  const int destHeight = dest->rows();
  const int destWidth = dest->cols();
  const int destStride = dest->rowBytes();
  uint8_t *const destBuf = dest->pixels();

  const float destWidthScale = srcHeight * 1.0f / destWidth;
  const float destHeightScale = srcWidth * 1.0f / destHeight;
  const float destScale = destWidthScale < destHeightScale ? destWidthScale : destHeightScale;

  const float cropWidthInSrc = destScale * destHeight;
  const float cropHeightInSrc = destScale * destWidth;

  const float srcRowStart = (srcHeight - cropHeightInSrc) / 2;
  const int srcColStart = (srcWidth - cropWidthInSrc) / 2;

  // Offset address of the first pixel in a row of the destination.
  //
  // The destination is rotated 90 degrees from the source, so we need to start writing to the
  // destination in the upper right column, moving downward with each pixel, and leftward with
  // each row.
  int destCol = destWidth - 1;

  for (int i = 0; i < destWidth; ++i) {
    // Set the current pixels to the start of the current row.
    int sourceRow = srcRowStart + i * destScale;

    const uint8_t *srcPix = srcBuf + sourceRow * srcStride + srcColStart;
    uint8_t *destPix = destBuf + destCol;
    float colOffset = 0;

    for (int j = 0; j < destHeight; ++j) {
      int colIndex = colOffset;
      // Copy the pixel.
      *destPix = srcPix[colIndex];

      colOffset += destScale;

      // We need to jump a row in the destination for every column in the source.
      destPix += destStride;
    }

    // Move left in the destination.
    destCol--;
  }
}

void downsizeAndRotate90Clockwise(const ConstTwoChannelPixels &src, TwoChannelPixels *dest) {
  if (src.rows() == dest->cols() && src.cols() == dest->rows()) {
    rotate90Clockwise(src, dest);
    return;
  }

  ScopeTimer t("downsize-r90-2channel");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int srcHeight = src.rows();
  const int srcWidth = src.cols();
  const int srcStride = src.rowBytes();
  const uint8_t *srcBuf = src.pixels();

  const int destHeight = dest->rows();
  const int destWidth = dest->cols();
  const int destStride = dest->rowBytes();
  uint8_t *const destBuf = dest->pixels();

  const float destWidthScale = srcHeight * 1.0f / destWidth;
  const float destHeightScale = srcWidth * 1.0f / destHeight;
  const float destScale = destWidthScale < destHeightScale ? destWidthScale : destHeightScale;

  const float cropWidthInSrc = destScale * destHeight;
  const float cropHeightInSrc = destScale * destWidth;

  const float srcRowStart = (srcHeight - cropHeightInSrc) / 2;
  const int srcColStart = (srcWidth - cropWidthInSrc) / 2;

  // Offset address of the first pixel in a row of the destination.
  //
  // The destination is rotated 90 degrees from the source, so we need to start writing to the
  // destination in the upper right column, moving downward with each pixel, and leftward with
  // each row.
  int destCol = destWidth - 1;

  for (int i = 0; i < destWidth; ++i) {
    // Set the current pixels to the start of the current row.
    int sourceRow = srcRowStart + i * destScale;

    const uint8_t *srcPix = srcBuf + sourceRow * srcStride + srcColStart + srcColStart;
    uint8_t *destPix = destBuf + destCol + destCol;
    float colOffset = 0;

    for (int j = 0; j < destHeight; ++j) {
      int colIndex = colOffset;
      // Copy the pixel.
      destPix[0] = srcPix[colIndex + colIndex];
      destPix[1] = srcPix[colIndex + colIndex + 1];

      colOffset += destScale;

      // We need to jump a row in the destination for every column in the source.
      destPix += destStride;
    }

    // Move left in the destination.
    destCol--;
  }
}

void downsizeAndRotate90Clockwise(
  const ConstUSkipPlanePixels &srcU, const ConstVSkipPlanePixels &srcV, TwoChannelPixels *dest) {
  if (srcU.rows() == dest->cols() && srcU.cols() == dest->rows()) {
    rotate90Clockwise(srcU, srcV, dest);
    return;
  }

  ScopeTimer t("downsize-r90-2-skipchannel");

  const int srcHeight = srcU.rows();
  const int srcWidth = srcU.cols();
  const int srcStride = srcU.rowBytes();
  const uint8_t *srcBuf1 = srcU.pixels();
  const uint8_t *srcBuf2 = srcV.pixels();

  downsizeAndRotate90ClockwiseIntoTwoChannel(
    srcHeight, srcWidth, srcStride, 2, srcBuf1, srcBuf2, dest);
}

void downsizeAndRotate90Clockwise(
  const ConstUPlanePixels &srcU, const ConstVPlanePixels &srcV, TwoChannelPixels *dest) {
  if (srcU.rows() == dest->cols() && srcU.cols() == dest->rows()) {
    rotate90Clockwise(srcU, srcV, dest);
    return;
  }

  ScopeTimer t("downsize-r90-2-singlechannel");

  const int srcHeight = srcU.rows();
  const int srcWidth = srcU.cols();
  const int srcStride = srcU.rowBytes();
  const uint8_t *srcBuf1 = srcU.pixels();
  const uint8_t *srcBuf2 = srcV.pixels();

  downsizeAndRotate90ClockwiseIntoTwoChannel(
    srcHeight, srcWidth, srcStride, 1, srcBuf1, srcBuf2, dest);
}

std::array<int32_t, 256> computeHistogram(ConstOneChannelPixels &src) {
  std::array<int32_t, 256> hist;
  std::fill(hist.begin(), hist.end(), 0);

  const int32_t srcStride = src.rowBytes();
  const uint8_t *srcRowStart = src.pixels();
  const uint8_t *srcRowEnd = src.pixels() + src.cols();
  const uint8_t *srcEnd = src.pixels() + src.rows() * srcStride;

  for (; srcRowStart < srcEnd; srcRowStart += srcStride, srcRowEnd += srcStride) {
    for (const uint8_t *pix = srcRowStart; pix < srcRowEnd; ++pix) {
      hist[*pix]++;
    }
  }

  return hist;
}

float estimateExposureScoreNEON(ConstOneChannelPixels &src) {
  float score = 0.0f;
#if defined(__ARM_NEON__) || FORCE_NEON_CODEPATH
  float scoreScale = 1.0 / (src.rows() * src.cols());
  const int jShift = 3 * 16;
  int leftOverCols = src.cols() % jShift;
  int colsAligned = src.cols() - leftOverCols;
// We load 16 pixel at a time, unroll to 3 step. This means we operate on 48 pixels at a time.
// Each comparison give us 0xFFFF (-1) on true, 0x0000 (0) on false.
// our score function was: score += 1 if val >= HIGH; score -= 1 if val <= LOW
// This is rewritten as: score -= -1 if val > HIGH - 1; score += -1 if val < LOW + 1
#define GET_VALUE(n) value##n = vld1q_u8(row_ptr + j + n * 16)
#define INIT(n) count##n = vmovq_n_s8(0)
#define COUNT_HIGH(n) count##n = vsubq_s8(count##n, vcgtq_u8(value##n, scoreHigh))
#define COUNT_LOW(n) count##n = vaddq_s8(count##n, vcltq_u8(value##n, scoreLow))
  uint8x16_t scoreHigh = vmovq_n_u8(EXPOSURE_SCORE_HIGH - 1);
  uint8x16_t scoreLow = vmovq_n_u8(EXPOSURE_SCORE_LOW + 1);
  const uint8_t *data = src.pixels();

  int i = 0;
  int j;
  const uint8_t *row_ptr = data;
  int8x16_t count0;
  int8x16_t count1;
  int8x16_t count2;
  while (i < src.rows()) {
    INIT(0);
    INIT(1);
    INIT(2);
    j = 0;
    uint8x16_t value0;
    uint8x16_t value1;
    uint8x16_t value2;
    while (j < colsAligned) {
      GET_VALUE(0);
      GET_VALUE(1);
      GET_VALUE(2);
      COUNT_HIGH(0);
      COUNT_HIGH(1);
      COUNT_HIGH(2);
      COUNT_LOW(0);
      COUNT_LOW(1);
      COUNT_LOW(2);
      j += jShift;
    }

#define ADD_LV_0(n) int16x8_t countX8_##n = vpaddlq_s8(count##n)
#define ADD_LV_1(n) int32x4_t countX4_##n = vpaddlq_s16(countX8_##n)
#define ACCUM(n) countAll = vaddq_s32(countAll, countX4_##n)

    // Collect count with tree add
    ADD_LV_0(0);
    ADD_LV_0(1);
    ADD_LV_0(2);
    ADD_LV_1(0);
    ADD_LV_1(1);
    ADD_LV_1(2);
    int32x4_t countAll = vmovq_n_s32(0);
    ACCUM(0);
    ACCUM(1);
    ACCUM(2);

    // Read out value into totalCount
    score += (vgetq_lane_s32(countAll, 0) + vgetq_lane_s32(countAll, 1)
              + vgetq_lane_s32(countAll, 2) + vgetq_lane_s32(countAll, 3))
      * scoreScale;

    // Handle the rest in non-SIMD
    if (leftOverCols > 0) {
      int leftOverCount = 0;
      for (int k = 0; k < leftOverCols; ++k) {
        uint8_t val = *(row_ptr + colsAligned + k);
        leftOverCount += (val >= EXPOSURE_SCORE_HIGH) - (val <= EXPOSURE_SCORE_LOW);
      }
      score += leftOverCount * scoreScale;
    }

    // Next row
    row_ptr += src.rowBytes();
    i += 1;
  }
#endif
  return score;
}

/** Estimate exposure by looking at the 1st channel of your image
 */
float estimateExposureScore(
  ConstOneChannelPixels &src, TaskQueue *taskQueue, ThreadPool *threadPool, int numChannels) {
  const int nTasks = src.rows() > 64 ? 16 : 1;
  int32_t scores[16];
  const int32_t rowInc = src.rowBytes() * (src.rows() / nTasks);

  auto scoreImage = [&](const uint8_t *_start, int i) {
    scores[i] = 0;
    const uint8_t *_end = _start + rowInc;
    const uint8_t *start = _start;
    const uint8_t *end = _start + src.cols() * numChannels;
    for (; start < _end; start += src.rowBytes(), end += src.rowBytes()) {
      for (const uint8_t *pix = start; pix < end; pix += numChannels) {
        scores[i] += (*pix >= EXPOSURE_SCORE_HIGH) - (*pix <= EXPOSURE_SCORE_LOW);
      }
    }
  };

  const uint8_t *rowStart = src.pixels();
  for (int i = 0; i < nTasks; ++i, rowStart += rowInc) {
    taskQueue->addTask(std::bind(std::cref(scoreImage), rowStart, i));
  }
  taskQueue->executeWithThreadPool(threadPool);

  int32_t score = 0;
  for (int ii = 0; ii < nTasks; ++ii) {
    score += scores[ii];
  }
  return static_cast<float>(score) / (src.cols() * src.rows());
}

/**
 * Asssuming that src.cols == dest.cols and src.rows == dest.rows
 */
void flipVertical(const ConstPixels &src, Pixels *dest) {
  ScopeTimer t("flip-vertical");
  int smallerRowBytes = std::min(src.rowBytes(), dest->rowBytes());

  for (int i = 0; i < src.rows(); i++) {
    memcpy(
      dest->pixels() + (i * dest->rowBytes()),
      src.pixels() + ((src.rows() - 1 - i) * src.rowBytes()),
      smallerRowBytes);
  }
}

void yuvToRgb(
  const ConstYPlanePixels &srcY, const ConstUVPlanePixels &srcUV, RGBA8888PlanePixels *dest) {
  ScopeTimer t("yuv-to-rgb");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int32_t srcWidth = srcY.cols();
  const int32_t srcHeight = srcY.rows();
  const int32_t srcYStride = srcY.rowBytes();
  const int32_t srcUVStride = srcUV.rowBytes();
  const uint8_t *const srcYPixels = srcY.pixels();
  const uint8_t *const srcUVPixels = srcUV.pixels();
  const int32_t destStride = dest->rowBytes();
  uint8_t *const destPixels = dest->pixels();

  // Offset address of the first pixel in a row of the Y plane.
  int srcYRowStart = 0;

  // Offset address of the first pixel in a row of the interleaved UV plane.
  int srcUVRowStart = 0;

  // Offset address of the first pixel in a row of the destination.
  int destRowStart = 0;

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    // Set the current pixels to the start of the current row.
    const uint8_t *srcYPix = srcYPixels + srcYRowStart;
    uint8_t *destPix = destPixels + destRowStart;

    const uint8_t *uvStart = srcUVPixels + srcUVRowStart;
    const uint8_t *uvEnd = uvStart + srcWidth;

    // Loop over UV value pairs. Each pair colors two y values.
    for (const uint8_t *srcUVPix = uvStart; srcUVPix < uvEnd; srcUVPix += 2) {
      // Extract the source YUV values.
      const uint8_t y0 = srcYPix[0];
      const uint8_t y1 = srcYPix[1];
      const uint8_t u = srcUVPix[0];
      const uint8_t v = srcUVPix[1];
      const auto rv = rvLut[v];
      const auto guv = guLut[u] + gvLut[v];
      const auto bu = buLut[u];

      // Use LUTs to efficiently compute:
      // uint8_t r = CLIP(y + 1.4f * (v-128));
      // uint8_t g = CLIP(y - 0.343f * (u-128) - 0.711f * (v-128));
      // uint8_t b = CLIP(y + 1.765f * (u-128));

      // Convert YUV to RGB, making sure to clip from 0 to 255.
      destPix[0] = uvClip[y0 + rv];
      destPix[1] = uvClip[y0 + guv];
      destPix[2] = uvClip[y0 + bu];

      destPix[4] = uvClip[y1 + rv];
      destPix[5] = uvClip[y1 + guv];
      destPix[6] = uvClip[y1 + bu];

      // Advance by two pixels in y and dest.
      srcYPix += 2;
      destPix += 8;
    }

    // Dest row advences by yStride every time.
    destRowStart += destStride;

    // Source row advances by yStride every time.
    srcYRowStart += srcYStride;

    // Interleaved source row advances by uStride (equals vStride) after odd rows.
    srcUVRowStart += srcUVStride * (sourceRow % 2);
  }
}

// Implementation was from the method above that works with interleaved UV.
void yuvToRgb(
  const ConstYPlanePixels &srcY,
  const ConstUPlanePixels &srcU,
  const ConstVPlanePixels &srcV,
  RGBA8888PlanePixels *dest) {
  ScopeTimer t("yuv-to-rgb");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int32_t srcWidth = srcY.cols();
  const int32_t srcHeight = srcY.rows();
  const int32_t srcYStride = srcY.rowBytes();
  const int32_t srcUStride = srcU.rowBytes();
  const int32_t srcVStride = srcV.rowBytes();
  const uint8_t *const srcYPixels = srcY.pixels();
  const uint8_t *const srcUPixels = srcU.pixels();
  const uint8_t *const srcVPixels = srcV.pixels();
  const int32_t destStride = dest->rowBytes();
  uint8_t *const destPixels = dest->pixels();

  // Offset address of the first pixel in a row of each plane.
  int srcYRowStart = 0;
  int srcURowStart = 0;
  int srcVRowStart = 0;

  // Offset address of the first pixel in a row of the destination.
  int destRowStart = 0;

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    // Set the current pixels to the start of the current row.
    const uint8_t *srcYPix = srcYPixels + srcYRowStart;
    uint8_t *destPix = destPixels + destRowStart;

    const uint8_t *uStart = srcUPixels + srcURowStart;
    const uint8_t *uEnd = uStart + srcWidth / 2;
    const uint8_t *vStart = srcVPixels + srcVRowStart;
    const uint8_t *vEnd = vStart + srcWidth / 2;

    // Loop over UV value. Each pair colors two y values.
    for (const uint8_t *srcUPix = uStart, *srcVPix = vStart; srcUPix < uEnd && srcVPix < vEnd;
         srcUPix++, srcVPix++) {
      // Extract the source YUV values.
      const uint8_t y0 = srcYPix[0];
      const uint8_t y1 = srcYPix[1];
      const uint8_t u = srcUPix[0];
      const uint8_t v = srcVPix[0];
      const auto rv = rvLut[v];
      const auto guv = guLut[u] + gvLut[v];
      const auto bu = buLut[u];

      // Convert YUV to RGB, making sure to clip from 0 to 255.
      destPix[0] = uvClip[y0 + rv];
      destPix[1] = uvClip[y0 + guv];
      destPix[2] = uvClip[y0 + bu];

      destPix[4] = uvClip[y1 + rv];
      destPix[5] = uvClip[y1 + guv];
      destPix[6] = uvClip[y1 + bu];

      // Advance by two pixels in y and dest.
      srcYPix += 2;
      destPix += 8;
    }

    // Dest row advences by yStride every time.
    destRowStart += destStride;

    // Source row advances by yStride every time.
    srcYRowStart += srcYStride;

    // Source row advances by uStride (equals vStride) after odd rows.
    srcURowStart += srcUStride * (sourceRow % 2);
    srcVRowStart += srcVStride * (sourceRow % 2);
  }
}

// This implementation is using using floating-point operations to convert YUV to RGBA for high precision
// This function leverages the BT.709 inverse color transformation matrix to convert YUV data to
// RGB, ensuring accurate color representation. The method processes individual YUV components
// and calculates RGB values using floating-point arithmetic, which avoids the limitations of
// lookup tables and provides a more precise color conversion. Each output pixel includes an
// alpha channel set to 255 (fully opaque).
void bt709ToRgbHighPrecision(
  const ConstYPlanePixels &srcY,
  const ConstUPlanePixels &srcU,
  const ConstVPlanePixels &srcV,
  RGBA8888PlanePixels *dest) {
  ScopeTimer t("yuv-to-rgb");

  constexpr float bt709InverseMatrix[3][3] = {
    {1.0f,  0.0f,       1.5748f},   // R = Y + 1.5748 * (V - 128)
    {1.0f, -0.187324f, -0.468124f}, // G = Y - 0.187324 * (U - 128) - 0.468124 * (V - 128)
    {1.0f,  1.8556f,    0.0f}       // B = Y + 1.8556 * (U - 128)
  };

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int32_t srcWidth = srcY.cols();
  const int32_t srcHeight = srcY.rows();
  const int32_t srcYStride = srcY.rowBytes();
  const int32_t srcUStride = srcU.rowBytes();
  const int32_t srcVStride = srcV.rowBytes();
  const uint8_t *const srcYPixels = srcY.pixels();
  const uint8_t *const srcUPixels = srcU.pixels();
  const uint8_t *const srcVPixels = srcV.pixels();
  const int32_t destStride = dest->rowBytes();
  uint8_t *const destPixels = dest->pixels();

  // Offset address of the first pixel in a row of each plane.
  int srcYRowStart = 0;
  int srcURowStart = 0;
  int srcVRowStart = 0;

  // Offset address of the first pixel in a row of the destination.
  int destRowStart = 0;

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    const uint8_t *srcYPix = srcYPixels + srcYRowStart;
    uint8_t *destPix = destPixels + destRowStart;

    const uint8_t *uStart = srcUPixels + srcURowStart;
    const uint8_t *vStart = srcVPixels + srcVRowStart;

    for (int i = 0; i < srcWidth; i += 2) {
        // Extract the source YUV values
        const float Y0 = static_cast<float>(srcYPix[i]);
        const float Y1 = static_cast<float>(srcYPix[i + 1]);
        const float U = static_cast<float>(uStart[i / 2]) - 128.0f; // Subtract 128 for chroma
        const float V = static_cast<float>(vStart[i / 2]) - 128.0f; // Subtract 128 for chroma

        // Convert YUV to RGB using the BT.709 matrix
        float R0 = bt709InverseMatrix[0][0] * Y0 + bt709InverseMatrix[0][1] * U + bt709InverseMatrix[0][2] * V;
        float G0 = bt709InverseMatrix[1][0] * Y0 + bt709InverseMatrix[1][1] * U + bt709InverseMatrix[1][2] * V;
        float B0 = bt709InverseMatrix[2][0] * Y0 + bt709InverseMatrix[2][1] * U + bt709InverseMatrix[2][2] * V;

        float R1 = bt709InverseMatrix[0][0] * Y1 + bt709InverseMatrix[0][1] * U + bt709InverseMatrix[0][2] * V;
        float G1 = bt709InverseMatrix[1][0] * Y1 + bt709InverseMatrix[1][1] * U + bt709InverseMatrix[1][2] * V;
        float B1 = bt709InverseMatrix[2][0] * Y1 + bt709InverseMatrix[2][1] * U + bt709InverseMatrix[2][2] * V;

        // Store RGBA values for two pixels
        destPix[0] = clampToByte(R0);  // Pixel 1 Red
        destPix[1] = clampToByte(G0);  // Pixel 1 Green
        destPix[2] = clampToByte(B0);  // Pixel 1 Blue
        destPix[3] = 255;              // Pixel 1 Alpha

        destPix[4] = clampToByte(R1);  // Pixel 2 Red
        destPix[5] = clampToByte(G1);  // Pixel 2 Green
        destPix[6] = clampToByte(B1);  // Pixel 2 Blue
        destPix[7] = 255;              // Pixel 2 Alpha

        destPix += 8; // Move to the next pair of destination pixels
    }

    // Advance row pointers
    destRowStart += destStride;
    srcYRowStart += srcYStride;

    // Subsampled U and V planes advance every two rows
    if (sourceRow % 2 == 1) {
        srcURowStart += srcUStride;
        srcVRowStart += srcVStride;
    }
  }
}

void yToRgb(const ConstYPlanePixels &srcY, RGBA8888PlanePixels *dest) {
  ScopeTimer t("y-to-rgb");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int32_t srcWidth = srcY.cols();
  const int32_t srcHeight = srcY.rows();
  const int32_t srcYStride = srcY.rowBytes();
  const uint8_t *const srcYPixels = srcY.pixels();
  const int32_t destStride = dest->rowBytes();
  uint8_t *const destPixels = dest->pixels();

  // Offset address of the first pixel in a row of the Y plane.
  int srcYRowStart = 0;

  // Offset address of the first pixel in a row of the destination.
  int destRowStart = 0;

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    // Set the current pixels to the start of the current row.
    const uint8_t *srcYStart = srcYPixels + srcYRowStart;
    const uint8_t *yEnd = srcYPixels + srcYRowStart + srcWidth;
    uint8_t *destPix = destPixels + destRowStart;

    // Loop over UV value pairs. Each pair colors two y values.
    for (const uint8_t *srcYPix = srcYStart; srcYPix < yEnd; ++srcYPix) {
      // Extract the source YUV values.
      const uint8_t y = *srcYPix;

      // Copy y value to all corner channels.
      destPix[0] = y;
      destPix[1] = y;
      destPix[2] = y;
      destPix[3] = 255;

      destPix += 4;
    }

    // Dest row advences by yStride every time.
    destRowStart += destStride;

    // Source row advances by yStride every time.
    srcYRowStart += srcYStride;
  }
}

void yuvToBgr(
  const ConstYPlanePixels &srcY, const ConstUVPlanePixels &srcUV, BGR888PlanePixels *dest) {
  ScopeTimer t("yuv-to-bgr");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int32_t srcWidth = srcY.cols();
  const int32_t srcHeight = srcY.rows();
  const int32_t srcYStride = srcY.rowBytes();
  const int32_t srcUVStride = srcUV.rowBytes();
  const uint8_t *const srcYPixels = srcY.pixels();
  const uint8_t *const srcUVPixels = srcUV.pixels();
  const int32_t destStride = dest->rowBytes();
  uint8_t *const destPixels = dest->pixels();

  // Offset address of the first pixel in a row of the Y plane.
  int srcYRowStart = 0;

  // Offset address of the first pixel in a row of the interleaved UV plane.
  int srcUVRowStart = 0;

  // Offset address of the first pixel in a row of the destination.
  int destRowStart = 0;

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    const uint8_t *srcYPix = srcYPixels + srcYRowStart;
    uint8_t *destPix = destPixels + destRowStart;

    const uint8_t *uvStart = srcUVPixels + srcUVRowStart;
    const uint8_t *uvEnd = uvStart + srcWidth;

    // Loop over UV value pairs. Each pair colors two y values.
    for (const uint8_t *srcUVPix = uvStart; srcUVPix < uvEnd; srcUVPix += 2) {
      // Extract the source YUV values.
      const uint8_t y0 = srcYPix[0];
      const uint8_t y1 = srcYPix[1];
      const uint8_t u = srcUVPix[0];
      const uint8_t v = srcUVPix[1];
      const auto rv = rvLut[v];
      const auto guv = guLut[u] + gvLut[v];
      const auto bu = buLut[u];

      // Use LUTs to efficiently compute:
      // uint8_t r = CLIP(y + 1.4f * (v-128));
      // uint8_t g = CLIP(y - 0.343f * (u-128) - 0.711f * (v-128));
      // uint8_t b = CLIP(y + 1.765f * (u-128));

      // Convert YUV to RGB, making sure to clip from 0 to 255.
      destPix[0] = uvClip[y0 + bu];
      destPix[1] = uvClip[y0 + guv];
      destPix[2] = uvClip[y0 + rv];

      destPix[3] = uvClip[y1 + bu];
      destPix[4] = uvClip[y1 + guv];
      destPix[5] = uvClip[y1 + rv];

      // Advance by two pixels in y and dest.
      srcYPix += 2;
      destPix += 6;
    }

    // Dest row advences by yStride every time.
    destRowStart += destStride;

    // Source row advances by yStride every time.
    srcYRowStart += srcYStride;

    // Interleaved source row advances by uStride (equals vStride) after odd
    // rows.
    srcUVRowStart += srcUVStride * (sourceRow % 2);
  }
}

namespace {
constexpr uint8_t CLIP(float v) {
  return static_cast<uint8_t>(v > 255.0 ? 255.0 : (v < 0.0 ? 0.0 : v));
}
}  // namespace

// Convert RGBA8888 to YUVA8888.
void rgbToYuv(const ConstRGBA8888PlanePixels &src, YUVA8888PlanePixels *dest) {
  ScopeTimer t("rgb-to-yuv");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int32_t srcWidth = src.cols();
  const int32_t srcHeight = src.rows();
  const int32_t srcStride = src.rowBytes();
  const uint8_t *const srcPixels = src.pixels();
  const int32_t destStride = dest->rowBytes();
  uint8_t *const destPixels = dest->pixels();

  int srcRowStart = 0;
  int destRowStart = 0;

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    const uint8_t *srcPix = srcPixels + srcRowStart;
    const uint8_t *srcRowEnd = srcPix + (4 * srcWidth);
    uint8_t *destPix = destPixels + destRowStart;

    while (srcPix < srcRowEnd) {
      const uint8_t r = srcPix[0];
      const uint8_t g = srcPix[1];
      const uint8_t b = srcPix[2];
      const uint8_t a = srcPix[3];

      uint8_t y = CLIP(.299 * r + .587 * g + .114 * b);
      uint8_t u = CLIP((-.14713 * r + -.28886 * g + .436 * b) + 128);
      uint8_t v = CLIP((.615 * r + -.51499 * g + -.10001 * b) + 128);

      destPix[0] = y;
      destPix[1] = u;
      destPix[2] = v;
      destPix[3] = a;

      srcPix += 4;
      destPix += 4;
    }

    destRowStart += destStride;
    srcRowStart += srcStride;
  }
}

void applyColorMatrixKeepAlpha(
  const HMatrix &mat, const ConstFourChannelPixels &src, FourChannelPixels *dest) {
  ScopeTimer t("apply-color-matrix-keep-alpha");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int32_t srcWidth = src.cols();
  const int32_t srcHeight = src.rows();
  const int32_t srcStride = src.rowBytes();
  const uint8_t *const srcPixels = src.pixels();
  const int32_t destStride = dest->rowBytes();
  uint8_t *const destPixels = dest->pixels();

  int srcRowStart = 0;
  int destRowStart = 0;

  const auto &m = mat.data();

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    const uint8_t *srcPix = srcPixels + srcRowStart;
    const uint8_t *srcRowEnd = srcPix + (4 * srcWidth);
    uint8_t *destPix = destPixels + destRowStart;

    while (srcPix < srcRowEnd) {
      const uint8_t r = srcPix[0];
      const uint8_t g = srcPix[1];
      const uint8_t b = srcPix[2];
      const uint8_t a = srcPix[3];

      float x = m[0] * r + m[4] * g + m[8] * b + m[12];
      float y = m[1] * r + m[5] * g + m[9] * b + m[13];
      float z = m[2] * r + m[6] * g + m[10] * b + m[14];

      destPix[0] = CLIP(x);
      destPix[1] = CLIP(y);
      destPix[2] = CLIP(z);
      destPix[3] = a;

      srcPix += 4;
      destPix += 4;
    }

    destRowStart += destStride;
    srcRowStart += srcStride;
  }
}

// Convert YUVA8888 to Y and UV.
void yuvToPlanarYuv(
  const ConstYUVA8888PlanePixels &src, YPlanePixels *yDest, UVPlanePixels *uvDest) {
  ScopeTimer t("yuva-to-yuv");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int32_t srcWidth = src.cols();
  const int32_t srcHeight = src.rows();
  const int32_t srcStride = src.rowBytes();
  const uint8_t *const srcPixels = src.pixels();

  const int32_t destYStride = yDest->rowBytes();
  uint8_t *const destYPixels = yDest->pixels();

  const int32_t destUVStride = uvDest->rowBytes();
  uint8_t *const destUVPixels = uvDest->pixels();

  int srcRowStart = 0;
  int destYRowStart = 0;
  int destUVRowStart = 0;

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    const uint8_t *srcPix = srcPixels + srcRowStart;

    uint8_t *destYPix = destYPixels + destYRowStart;

    uint8_t *uvStart = destUVPixels + destUVRowStart;
    uint8_t *uvEnd = uvStart + srcWidth;  // 2x channels @ half width

    // Loop over UV value pairs. Each pair colors two y values.
    for (uint8_t *destUVPix = uvStart; destUVPix < uvEnd; destUVPix += 2) {
      // Extract y values for two pixels and uv values for one uv pixel.
      const uint8_t y0 = srcPix[0];
      const uint8_t u = srcPix[1];
      const uint8_t v = srcPix[2];
      const uint8_t y1 = srcPix[4];

      // Copy data to two y pixels and 1 uv pixel.
      destYPix[0] = y0;
      destYPix[1] = y1;
      destUVPix[0] = u;
      destUVPix[1] = v;

      // Advance by two pixels in src and destY.
      srcPix += 8;
      destYPix += 2;
    }

    // Dest row advences by yStride every time.
    destYRowStart += destYStride;

    // Source row advances by yStride every time.
    srcRowStart += srcStride;

    // Interleaved source row advances by uStride (equals vStride) after odd rows.
    destUVRowStart += destUVStride * (sourceRow % 2);
  }
}

// Convert RGB888 to grayscale Y.
void rgbToGray(const ConstRGB888PlanePixels &src, YPlanePixels *dest) {
  ScopeTimer t("rgb-to-gray");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int32_t srcWidth = src.cols();
  const int32_t srcHeight = src.rows();
  const int32_t srcStride = src.rowBytes();
  const uint8_t *const srcPixels = src.pixels();
  const int32_t destStride = dest->rowBytes();
  uint8_t *const destPixels = dest->pixels();

  int srcRowStart = 0;
  int destRowStart = 0;

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    const uint8_t *srcPix = srcPixels + srcRowStart;
    const uint8_t *srcRowEnd = srcPix + (3 * srcWidth);
    uint8_t *destPix = destPixels + destRowStart;

    while (srcPix < srcRowEnd) {
      const uint8_t r = srcPix[0];
      const uint8_t g = srcPix[1];
      const uint8_t b = srcPix[2];

      uint8_t y = yr[r] + yg[g] + yb[b];
      destPix[0] = y;

      srcPix += 3;
      destPix += 1;
    }

    destRowStart += destStride;
    srcRowStart += srcStride;
  }
}

// Convert RGBA8888 to grayscale Y.
void rgbToGray(const ConstRGBA8888PlanePixels &src, YPlanePixels *dest) {
  ScopeTimer t("rgb-to-gray");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int32_t srcWidth = src.cols();
  const int32_t srcHeight = src.rows();
  const int32_t srcStride = src.rowBytes();
  const uint8_t *const srcPixels = src.pixels();
  const int32_t destStride = dest->rowBytes();
  uint8_t *const destPixels = dest->pixels();

  int srcRowStart = 0;
  int destRowStart = 0;

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    const uint8_t *srcPix = srcPixels + srcRowStart;
    const uint8_t *srcRowEnd = srcPix + (4 * srcWidth);
    uint8_t *destPix = destPixels + destRowStart;

    while (srcPix < srcRowEnd) {
      const uint8_t r = srcPix[0];
      const uint8_t g = srcPix[1];
      const uint8_t b = srcPix[2];

      uint8_t y = yr[r] + yg[g] + yb[b];
      destPix[0] = y;

      srcPix += 4;
      destPix += 1;
    }

    destRowStart += destStride;
    srcRowStart += srcStride;
  }
}

// Convert BGR888 to grayscale Y.
void bgrToGray(const ConstBGR888PlanePixels &src, YPlanePixels *dest) {
  ScopeTimer t("bgr-to-gray");

  // Copy reference values onto the stack so the compiler can infer constness across loops.
  const int32_t srcWidth = src.cols();
  const int32_t srcHeight = src.rows();
  const int32_t srcStride = src.rowBytes();
  const uint8_t *const srcPixels = src.pixels();
  const int32_t destStride = dest->rowBytes();
  uint8_t *const destPixels = dest->pixels();

  int srcRowStart = 0;
  int destRowStart = 0;

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    const uint8_t *srcPix = srcPixels + srcRowStart;
    const uint8_t *srcRowEnd = srcPix + (3 * srcWidth);
    uint8_t *destPix = destPixels + destRowStart;

    while (srcPix < srcRowEnd) {
      const uint8_t b = srcPix[0];
      const uint8_t g = srcPix[1];
      const uint8_t r = srcPix[2];

      uint8_t y = yr[r] + yg[g] + yb[b];
      destPix[0] = y;

      srcPix += 3;
      destPix += 1;
    }

    destRowStart += destStride;
    srcRowStart += srcStride;
  }
}

std::array<float, 1> meanPixelValue(ConstOneChannelPixels src) {
  std::array<int32_t, 1> sum{{0}};
  for (int i = 0; i < src.rows(); ++i) {
    const uint8_t *srcb = src.pixels() + i * src.rowBytes();
    for (int j = 0; j < src.cols(); ++j) {
      sum[0] += srcb[0];
      srcb += 1;
    }
  }
  float den = 1.0f / (src.rows() * src.cols());
  return std::array<float, 1>{{sum[0] * den}};
}

std::array<float, 2> meanPixelValue(ConstTwoChannelPixels src) {
  std::array<int32_t, 2> sum{{0, 0}};
  for (int i = 0; i < src.rows(); ++i) {
    const uint8_t *srcb = src.pixels() + i * src.rowBytes();
    for (int j = 0; j < src.cols(); ++j) {
      sum[0] += srcb[0];
      sum[1] += srcb[1];
      srcb += 2;
    }
  }
  float den = 1.0f / (src.rows() * src.cols());
  return std::array<float, 2>{{sum[0] * den, sum[1] * den}};
}

std::array<float, 3> meanPixelValue(ConstThreeChannelPixels src) {
  std::array<int32_t, 3> sum{{0, 0, 0}};
  for (int i = 0; i < src.rows(); ++i) {
    const uint8_t *srcb = src.pixels() + i * src.rowBytes();
    for (int j = 0; j < src.cols(); ++j) {
      sum[0] += srcb[0];
      sum[1] += srcb[1];
      sum[2] += srcb[2];
      srcb += 3;
    }
  }
  float den = 1.0f / (src.rows() * src.cols());
  return std::array<float, 3>{{sum[0] * den, sum[1] * den, sum[2] * den}};
}

std::array<float, 4> meanPixelValue(ConstFourChannelPixels src) {
  std::array<int32_t, 4> sum{{0, 0, 0, 0}};
  for (int i = 0; i < src.rows(); ++i) {
    const uint8_t *srcb = src.pixels() + i * src.rowBytes();
    for (int j = 0; j < src.cols(); ++j) {
      sum[0] += srcb[0];
      sum[1] += srcb[1];
      sum[2] += srcb[2];
      sum[3] += srcb[3];
      srcb += 4;
    }
  }
  float den = 1.0f / (src.rows() * src.cols());
  return std::array<float, 4>{{sum[0] * den, sum[1] * den, sum[2] * den, sum[3] * den}};
}

void bgrToRgba(const ConstBGR888PlanePixels &src, RGBA8888PlanePixels *dest) {
  ScopeTimer t("bgr-to-rgba");
  const uint8_t *srcRowStart = src.pixels();
  uint8_t *destRowStart = dest->pixels();
  for (int i = 0; i < src.rows(); ++i) {
    const uint8_t *srcPix = srcRowStart;
    uint8_t *destPix = destRowStart;
    for (int j = 0; j < src.cols(); ++j) {
      destPix[0] = srcPix[2];
      destPix[1] = srcPix[1];
      destPix[2] = srcPix[0];
      srcPix += 3;
      destPix += 4;
    }
    srcRowStart += src.rowBytes();
    destRowStart += dest->rowBytes();
  }
}

void rgbToBgr(const ConstRGB888PlanePixels &src, BGR888PlanePixels *dest) {
  ScopeTimer t("rgb-to-bgr");
  const uint8_t *srcRowStart = src.pixels();
  uint8_t *destRowStart = dest->pixels();
  for (int i = 0; i < src.rows(); ++i) {
    const uint8_t *srcPix = srcRowStart;
    uint8_t *destPix = destRowStart;
    for (int j = 0; j < src.cols(); ++j) {
      destPix[0] = srcPix[2];
      destPix[1] = srcPix[1];
      destPix[2] = srcPix[0];
      srcPix += 3;
      destPix += 3;
    }
    srcRowStart += src.rowBytes();
    destRowStart += dest->rowBytes();
  }
}

void rgbaToBgr(const ConstRGBA8888PlanePixels &src, BGR888PlanePixels *dest) {
  ScopeTimer t("rgba-to-bgr");
  const uint8_t *srcRowStart = src.pixels();
  uint8_t *destRowStart = dest->pixels();
  for (int i = 0; i < src.rows(); ++i) {
    const uint8_t *srcPix = srcRowStart;
    uint8_t *destPix = destRowStart;
    for (int j = 0; j < src.cols(); ++j) {
      destPix[0] = srcPix[2];
      destPix[1] = srcPix[1];
      destPix[2] = srcPix[0];
      srcPix += 4;
      destPix += 3;
    }
    srcRowStart += src.rowBytes();
    destRowStart += dest->rowBytes();
  }
}

void rgbaToBgra(const ConstRGBA8888PlanePixels &src, BGRA8888PlanePixels *dest) {
  ScopeTimer t("rgba-to-bgra");
  const uint8_t *srcRowStart = src.pixels();
  uint8_t *destRowStart = dest->pixels();
  for (int i = 0; i < src.rows(); ++i) {
    const uint8_t *srcPix = srcRowStart;
    uint8_t *destPix = destRowStart;
    for (int j = 0; j < src.cols(); ++j) {
      destPix[0] = srcPix[2];
      destPix[1] = srcPix[1];
      destPix[2] = srcPix[0];
      destPix[3] = srcPix[3];
      srcPix += 4;
      destPix += 4;
    }
    srcRowStart += src.rowBytes();
    destRowStart += dest->rowBytes();
  }
}

void floatToRgba(
  const ConstFloatPixels &src,
  RGBA8888PlanePixels *dest,
  const uint8_t colormap[256][3],
  float min,
  float max) {
  ScopeTimer t("float-to-rgba");
  const float *srcRowStart = src.pixels();
  uint8_t *destRowStart = dest->pixels();
  float scale = 255 / (max - min);

  for (int i = 0; i < src.rows(); ++i) {
    const float *srcPix = srcRowStart;
    uint8_t *destPix = destRowStart;
    for (int j = 0; j < src.cols(); ++j) {
      // Linear scale float to the range [0, 255].
      // Anything out of the parameter range will be forced to be 0 or 255.
      uint8_t destPixValue = CLIP((*srcPix - min) * scale);

      // Set the rgba value.
      const auto *color = colormap[destPixValue];
      destPix[0] = color[0];
      destPix[1] = color[1];
      destPix[2] = color[2];
      destPix[3] = 255;
      srcPix += 1;
      destPix += 4;
    }
    srcRowStart += src.rowElements();
    destRowStart += dest->rowBytes();
  }
}

void floatToRgbaGray(const ConstFloatPixels &src, RGBA8888PlanePixels *dest, float min, float max) {
  ScopeTimer t("float-to-rgba-gray");

  floatToRgba(src, dest, GRAYSCALE_RGB_256, min, max);
}

void floatToRgbaRemoveLetterbox(
  const ConstFloatPixels &src,
  RGBA8888PlanePixels *dest,
  const uint8_t colormap[256][3],
  float min,
  float max) {
  ScopeTimer t("float-to-rgba-remove-letterbox");
  int srcRowOffset = (src.rows() - dest->rows()) / 2;
  int srcColOffset = (src.cols() - dest->cols()) / 2;

  int rows = std::min(dest->rows(), src.rows() - srcRowOffset);
  int cols = std::min(dest->cols(), src.cols() - srcColOffset);

  const float *srcRowStart = src.pixels();
  srcRowStart += srcRowOffset * src.rowElements();
  srcRowStart += srcColOffset;
  uint8_t *destRowStart = dest->pixels();
  float scale = 255 / (max - min);

  for (int i = 0; i < rows; ++i) {
    const float *srcPix = srcRowStart;
    uint8_t *destPix = destRowStart;
    for (int j = 0; j < cols; ++j) {
      // Linear scale float to the range [0, 255].
      // Anything out of the parameter range will be forced to be 0 or 255.
      uint8_t destPixValue = CLIP((*srcPix - min) * scale);

      // Set the rgba value.
      const auto *color = colormap[destPixValue];
      destPix[0] = color[0];
      destPix[1] = color[1];
      destPix[2] = color[2];
      destPix[3] = 255;
      srcPix += 1;
      destPix += 4;
    }
    srcRowStart += src.rowElements();
    destRowStart += dest->rowBytes();
  }
}

void floatToRgbaGrayRemoveLetterbox(
  const ConstFloatPixels &src, RGBA8888PlanePixels *dest, float min, float max) {
  floatToRgbaRemoveLetterbox(src, dest, GRAYSCALE_RGB_256, min, max);
}

void floatToOneChannelFloatRescale0To1(
  const ConstOneChannelFloatPixels &src, OneChannelFloatPixels *dest, float srcMin, float srcMax) {
  if (dest == nullptr || src.rows() * src.cols() != dest->rows() * dest->cols()) {
    return;
  }

  const float range = srcMax - srcMin;
  const float invR = 1.0f / range;
  const float *srcPtr = src.pixels();
  float *dstPtr = dest->pixels();
  for (auto i = 0; i < src.rows() * src.cols(); ++i) {
    float v = srcPtr[i];
    v = std::min(std::max(v, srcMin), srcMax);
    dstPtr[i] = invR * (v - srcMin);
  }
}

void fill(uint8_t a, OneChannelPixels *dest) {
  ScopeTimer t("fill1");
  uint8_t *destRowStart = dest->pixels();
  for (int i = 0; i < dest->rows(); ++i) {
    std::fill(destRowStart, destRowStart + dest->cols(), a);
    destRowStart += dest->rowBytes();
  }
}

void fill(uint8_t a, uint8_t b, TwoChannelPixels *dest) {
  ScopeTimer t("fill2");
  uint8_t *destRowStart = dest->pixels();
  // Optimize case of single value.
  if (a == b) {
    for (int i = 0; i < dest->rows(); ++i) {
      std::fill(destRowStart, destRowStart + 2 * dest->cols(), a);
      destRowStart += dest->rowBytes();
    }
    return;
  }
  for (int i = 0; i < dest->rows(); ++i) {
    uint8_t *destPix = destRowStart;
    for (int j = 0; j < dest->cols(); ++j) {
      destPix[0] = a;
      destPix[1] = b;
      destPix += 2;
    }
    destRowStart += dest->rowBytes();
  }
}

void fill(uint8_t a, uint8_t b, uint8_t c, ThreeChannelPixels *dest) {
  ScopeTimer t("fill3");
  uint8_t *destRowStart = dest->pixels();
  // Optimize case of single value.
  if (a == b && a == c) {
    for (int i = 0; i < dest->rows(); ++i) {
      std::fill(destRowStart, destRowStart + 3 * dest->cols(), a);
      destRowStart += dest->rowBytes();
    }
    return;
  }
  for (int i = 0; i < dest->rows(); ++i) {
    uint8_t *destPix = destRowStart;
    for (int j = 0; j < dest->cols(); ++j) {
      destPix[0] = a;
      destPix[1] = b;
      destPix[2] = c;
      destPix += 3;
    }
    destRowStart += dest->rowBytes();
  }
}

void fill(uint8_t a, uint8_t b, uint8_t c, uint8_t d, FourChannelPixels *dest) {
  ScopeTimer t("fill4");
  uint8_t *destRowStart = dest->pixels();
  // Optimize case of single value.
  if (a == b && a == c && a == d) {
    for (int i = 0; i < dest->rows(); ++i) {
      std::fill(destRowStart, destRowStart + 4 * dest->cols(), a);
      destRowStart += dest->rowBytes();
    }
    return;
  }
  for (int i = 0; i < dest->rows(); ++i) {
    uint8_t *destPix = destRowStart;
    for (int j = 0; j < dest->cols(); ++j) {
      destPix[0] = a;
      destPix[1] = b;
      destPix[2] = c;
      destPix[3] = d;
      destPix += 4;
    }
    destRowStart += dest->rowBytes();
  }
}

YPlanePixels crop(YPlanePixels src, int h_, int w_, int y_, int x_) {
  int x = std::min(std::max(x_, 0), src.cols());
  int y = std::min(std::max(y_, 0), src.rows());
  int w = std::min(std::max(w_, 0), src.cols() - x);
  int h = std::min(std::max(h_, 0), src.rows() - y);
  return YPlanePixels(h, w, src.rowBytes(), src.pixels() + y * src.rowBytes() + x);
}

RGBA8888PlanePixels crop(RGBA8888PlanePixels src, int h_, int w_, int y_, int x_) {
  int x = std::min(std::max(x_, 0), src.cols());
  int y = std::min(std::max(y_, 0), src.rows());
  int w = std::min(std::max(w_, 0), src.cols() - x);
  int h = std::min(std::max(h_, 0), src.rows() - y);
  return RGBA8888PlanePixels(h, w, src.rowBytes(), src.pixels() + y * src.rowBytes() + 4 * x);
}

bool isAllZeroes(ConstFourChannelPixels src, int channel) {
  for (int i = 0; i < src.rows(); ++i) {
    bool isNonZero = false;
    const uint8_t *srcb = src.pixels() + i * src.rowBytes();
    for (int j = 0; j < src.cols(); ++j) {
      isNonZero |= srcb[channel];
      srcb += 4;
    }
    if (isNonZero) {
      return false;
    }
  }
  return true;
}

void letterboxDimensionSameAspectRatio(
  int inputWidth,
  int inputHeight,
  int outputWidth,
  int outputHeight,
  int *letterboxWidth,
  int *letterboxHeight) {
  float fInWidth = static_cast<float>(inputWidth);
  float fInHeight = static_cast<float>(inputHeight);

  // compute the minimum resize ratio
  float wRatio = static_cast<float>(outputWidth) / fInWidth;
  float hRatio = static_cast<float>(outputHeight) / fInHeight;

  float ratio = (hRatio < wRatio) ? hRatio : wRatio;
  *letterboxWidth = static_cast<int>(std::floor(ratio * fInWidth));
  *letterboxHeight = static_cast<int>(std::floor(ratio * fInHeight));
}

void cropDimensionByAspectRatio(
  int inputWidth, int inputHeight, float aspectRatio, int *cropWidth, int *cropHeight) {
  if (!cropWidth || !cropHeight || aspectRatio < 1.0e-5) {
    return;
  }
  float newWidth = inputHeight * aspectRatio;
  if (newWidth < inputWidth) {
    *cropWidth = static_cast<int>(newWidth);
    *cropHeight = inputHeight;
  } else {
    *cropWidth = inputWidth;
    float newHeight = inputWidth / aspectRatio;
    *cropHeight = static_cast<int>(newHeight);
  }
}

void toLetterboxRGBFloatN1to1(
  ConstRGBA8888PlanePixels src, const int dstCols, const int dstRows, float *dst) {
  if (dst == nullptr) {
    return;
  }

  auto rows = src.rows();
  auto cols = src.cols();
  auto rowBytes = src.rowBytes();
  const auto *pixels = src.pixels();

  int rstart = std::floor((dstRows - rows) * 0.5f);
  int cstart = std::floor((dstCols - cols) * 0.5f);

  std::fill(dst, dst + dstRows * dstCols * 3, -1.0f);

  int sstart = 0;
  int dstart = (rstart * dstCols + cstart) * 3;
  for (int r = 0; r < rows; ++r) {
    int sidx = sstart;
    int didx = dstart;
    for (int c = 0; c < cols; ++c) {
      dst[didx + 0] = fn1to1[pixels[sidx + 0]];
      dst[didx + 1] = fn1to1[pixels[sidx + 1]];
      dst[didx + 2] = fn1to1[pixels[sidx + 2]];
      didx += 3;
      sidx += 4;
    }
    sstart += rowBytes;
    dstart += dstCols * 3;
  }
}

void toLetterboxRGBFloat0To1(
  ConstRGBA8888PlanePixels src, const int dstCols, const int dstRows, float *dst) {
  if (dst == nullptr) {
    return;
  }

  auto rows = src.rows();
  auto cols = src.cols();
  auto rowBytes = src.rowBytes();
  const auto *pixels = src.pixels();

  int rstart = std::floor((dstRows - rows) * 0.5f);
  int cstart = std::floor((dstCols - cols) * 0.5f);

  std::fill(dst, dst + dstRows * dstCols * 3, 0.0f);

  int sstart = 0;
  int dstart = (rstart * dstCols + cstart) * 3;
  for (int r = 0; r < rows; ++r) {
    int sidx = sstart;
    int didx = dstart;
    for (int c = 0; c < cols; ++c) {
      dst[didx + 0] = f0to1[pixels[sidx + 0]];
      dst[didx + 1] = f0to1[pixels[sidx + 1]];
      dst[didx + 2] = f0to1[pixels[sidx + 2]];
      didx += 3;
      sidx += 4;
    }
    sstart += rowBytes;
    dstart += dstCols * 3;
  }
}

void toLetterboxRGBFloat0To1FlipX(
  ConstRGBA8888PlanePixels src, const int dstCols, const int dstRows, float *dst) {
  if (dst == nullptr) {
    return;
  }

  auto rows = src.rows();
  auto cols = src.cols();
  auto rowBytes = src.rowBytes();
  const auto *pixels = src.pixels();

  int rstart = std::floor((dstRows - rows) * 0.5f);
  int cstart = std::floor((dstCols - cols) * 0.5f);

  std::fill(dst, dst + dstRows * dstCols * 3, 0.0f);

  int sstart = 0;
  int dstart = (rstart * dstCols + cstart) * 3;
  for (int r = 0; r < rows; ++r) {
    int sidx = sstart + 4 * (cols - 1);
    int didx = dstart;
    for (int c = 0; c < cols; ++c) {
      dst[didx + 0] = f0to1[pixels[sidx + 0]];
      dst[didx + 1] = f0to1[pixels[sidx + 1]];
      dst[didx + 2] = f0to1[pixels[sidx + 2]];
      didx += 3;
      sidx -= 4;
    }
    sstart += rowBytes;
    dstart += dstCols * 3;
  }
}

// TODO(dat): Add unit test
void stretchDepthValues(ConstDepthFloatPixels src, RGBA8888PlanePixels dst, float upperBound) {
  float maxVal = std::numeric_limits<float>::min();
  float minVal = std::numeric_limits<float>::max();
  auto *srcPtr = src.pixels();
  {
    for (size_t i = 0; i < src.rows(); i++) {
      for (size_t j = 0; j < src.cols(); j++) {
        auto val = srcPtr[i * src.rowElements() + j * 1];
        if (val > upperBound) {
          continue;
        }

        if (maxVal < val) {
          maxVal = val;
        }
        if (minVal > val) {
          minVal = val;
        }
      }
    }
  }

  auto *dstPtr = dst.pixels();
  {
    float scale = (maxVal > minVal) ? 255.f / (maxVal - minVal) : 1.f;
    for (size_t i = 0; i < dst.rows(); i++) {
      for (size_t j = 0; j < dst.cols(); j++) {
        float val = srcPtr[i * src.rowElements() + j * 1];
        uint8_t scaledVal = val > upperBound ? 0 : (val - minVal) * scale;
        dstPtr[i * dst.rowBytes() + j * 4] = scaledVal;
        dstPtr[i * dst.rowBytes() + j * 4 + 1] = scaledVal;
        dstPtr[i * dst.rowBytes() + j * 4 + 2] = scaledVal;
        dstPtr[i * dst.rowBytes() + j * 4 + 3] = 255;
      }
    }
  }
}

void decodeToDepth(ConstRGBA8888PlanePixels src, DepthFloatPixels dst, float near, float far) {
  auto *srcPtr = src.pixels();
  auto *dstPtr = dst.pixels();
  float scale = far - near;
  for (size_t i = 0; i < dst.rows(); i++) {
    for (size_t j = 0; j < dst.cols(); j++) {
      uint8_t x = srcPtr[i * src.rowBytes() + j * 4];
      uint8_t y = srcPtr[i * src.rowBytes() + j * 4 + 1];
      uint8_t z = srcPtr[i * src.rowBytes() + j * 4 + 2];
      uint8_t a = srcPtr[i * src.rowBytes() + j * 4 + 3];
      float decodedVal = decodeFloat(x, y, z, a);
      float scaledVal = decodedVal * scale;
      dstPtr[i * dst.rowElements() + j * 1] = scaledVal;
    }
  }
}

}  // namespace c8
