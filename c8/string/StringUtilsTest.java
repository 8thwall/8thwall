package com.the8thwall.c8.string;

import org.junit.Test;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

/**
 * Unit tests for {@link StringUtils}.
 */
public class StringUtilsTest {

  @Test
  public void testIsNullOrEmptyWithNullInput() {
    assertTrue(StringUtils.isNullOrEmpty(null));
  }

  @Test
  public void testIsNullOrEmptyWithEmptyInput() {
    assertTrue(StringUtils.isNullOrEmpty(""));
  }

  @Test
  public void testIsNullOrEmptyWithSpacesOnlyInput() {
    assertFalse(StringUtils.isNullOrEmpty("   "));
  }

  @Test
  public void testIsNullOrEmptyWithNonEmptyInput() {
    assertFalse(StringUtils.isNullOrEmpty("Alvin's name will forever live in this codebase"));
  }

  @Test
  public void testIsNullOrTrimmedEmptyWithNull() {
    assertTrue(StringUtils.isNullOrTrimmedEmpty(null));
  }

  @Test
  public void testIsNullOrTrimmedEmptyWithEmptyInput() {
    assertTrue(StringUtils.isNullOrTrimmedEmpty(""));
  }

  @Test
  public void testIsNullOrTrimmedEmptyWithSpacesOnly() {
    assertTrue(StringUtils.isNullOrTrimmedEmpty("           "));
  }

  @Test
  public void testIsNullOrTrimmedEmptyWithNonEmptyInput() {
    assertFalse(StringUtils.isNullOrTrimmedEmpty("  blah blah blah  "));
  }

}
