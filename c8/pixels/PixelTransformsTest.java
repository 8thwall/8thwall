package com.the8thwall.c8.pixels;

import org.junit.Test;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertNull;

/**
 * Unit tests for {@link PixelTransforms}.
 */
public class PixelTransformsTest {

  @Test
  public void testFlipVerticalWithFourChannelBytes() {
    byte[] bytes = {0, 1, 2, 3,
                    4, 5, 6, 7,
                    8, 9, 10, 11};
    PixelTransforms.flipVertical(bytes, 1, 3, 4);
    byte[] expectedBytes = {8, 9, 10, 11,
                            4, 5, 6, 7,
                            0, 1, 2, 3};
    assertArrayEquals(bytes, expectedBytes);
  }

  @Test
  public void testFlipVerticalWithTwoChannelBytes() {
    byte[] bytes = {0, 1, 2, 3,
                    4, 5, 6, 7,
                    8, 9, 10, 11};
    PixelTransforms.flipVertical(bytes, 2, 3, 2);
    byte[] expectedBytes = {8, 9, 10, 11,
                            4, 5, 6, 7,
                            0, 1, 2, 3};
    assertArrayEquals(bytes, expectedBytes);
  }

  @Test
  public void testFlipVerticalWithThreeChannelBytes() {
    byte[] bytes = {0, 1, 2,
                    8, 9, 10};
    PixelTransforms.flipVertical(bytes, 1, 2, 3);
    byte[] expectedBytes = {8, 9, 10,
                            0, 1, 2};
    assertArrayEquals(bytes, expectedBytes);
  }

  @Test
  public void testRgbToArgbWithInvalid3Channel() {
    byte[] bytes = {0, 1};
    int[] pixels = PixelTransforms.rgbToArgb(bytes);
    assertNull(pixels);
  }

  @Test
  public void testRgbToArgbWithValid3Channel() {
    byte[] bytes = {(byte) 0xd2, (byte) 0xab, 0x01, 0x21, 0x45, (byte) 0xff};
    int[] pixels = PixelTransforms.rgbToArgb(bytes);
    int[] expected = {0xffd2ab01, 0xff2145ff};
    assertArrayEquals(pixels, expected);
  }
}
