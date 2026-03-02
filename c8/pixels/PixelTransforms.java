package com.the8thwall.c8.pixels;

/**
 * Operations that produce images from images.
 */
public class PixelTransforms {

  /**
   * Flips the provided image byte vertically. This consists of swapping each row, R, with
   * its opposite, height - R.
   *
   * | 1 2 3 |     | 7 8 9 |
   * | 4 5 6 | --> | 4 5 6 |
   * | 7 8 9 |     | 1 2 3 |
   */
  public static void flipVertical(byte[] pixels, int width, int height, int pixelStride) {
    int rowStride = width * pixelStride;
    for (int row = 0; row < (height + 1) / 2; ++row) {
      for (int col = 0; col < rowStride; ++col) {
        int index = (row * rowStride) + col;
        int flipIndex = ((height - row - 1) * rowStride) + col;
        byte tmp = pixels[index];
        pixels[index] = pixels[flipIndex];
        pixels[flipIndex] = tmp;
      }
    }
  }

  /**
   * Converts the provided 3-channel pixel byte buffer into a 4-channel pixel int buffer, where
   * each int represents a single pixel in ARGB8888 format.
   */
  public static int[] rgbToArgb(byte[] pixels) {
    if (pixels.length % 3 != 0) {
      return null;
    }

    int[] intPixels = new int[pixels.length / 3];
    int intIndex = 0;
    int byteIndex = 0;
    while (byteIndex < pixels.length) {
      int a = 0xff << 24;
      int r = (pixels[byteIndex] & 0xff) << 16;
      int g = (pixels[byteIndex + 1] & 0xff) << 8;
      int b = (pixels[byteIndex + 2] & 0xff);

      int pixel = a | r | g | b;
      intPixels[intIndex] = pixel;

      ++intIndex;
      byteIndex += 3;
    }

    return intPixels;
  }
}
