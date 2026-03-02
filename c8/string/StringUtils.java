package com.the8thwall.c8.string;

/**
 * Utility class providing commonly logic commonly performed on {@code String}s.
 */
public class StringUtils {

  /**
   * Returns whether the given string is null or empty.
   */
  public static boolean isNullOrEmpty(String str) {
    return str == null || str.isEmpty();
  }

  /**
   * Retruns whether the given string is null or if it is empty after trimming.
   */
  public static boolean isNullOrTrimmedEmpty(String str) {
    return str == null || str.trim().isEmpty();
  }
}
