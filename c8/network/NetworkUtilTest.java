package com.the8thwall.c8.network;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.RuntimeEnvironment;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.fail;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

/**
 * Unit tests for {@link NetworkUtil}.
 */
@RunWith(RobolectricTestRunner.class)
public class NetworkUtilTest {

  private Context context_;

  @Before
  public void setup() {
    context_ = mock(Context.class);
  }

  @Test
  public void testHasNetworkPermissionsAndConnectivityWithoutNetwork() {
    ConnectivityManager connectivityManager = mock(ConnectivityManager.class);
    mockPermissionGranted(Manifest.permission.ACCESS_NETWORK_STATE);
    when(context_.getSystemService(Context.CONNECTIVITY_SERVICE)).thenReturn(connectivityManager);
    assertFalse(NetworkUtil.hasNetworkPermissionsAndConnectivity(context_));
  }

  @Test
  public void testHasNetworkPermissionsAndConnectivityWithoutAvailableNetwork() {
    ConnectivityManager connectivityManager = createMockConnectivityManager(false, false, false);
    mockPermissionGranted(Manifest.permission.ACCESS_NETWORK_STATE);
    when(context_.getSystemService(Context.CONNECTIVITY_SERVICE)).thenReturn(connectivityManager);
    assertFalse(NetworkUtil.hasNetworkPermissionsAndConnectivity(context_));
  }

  @Test
  public void testHasNetworkPermissionsAndConnectivityWithoutConnectivity() {
    ConnectivityManager connectivityManager = createMockConnectivityManager(true, false, false);
    when(context_.getSystemService(Context.CONNECTIVITY_SERVICE)).thenReturn(connectivityManager);
    assertFalse(NetworkUtil.hasNetworkPermissionsAndConnectivity(context_));
  }

  @Test
  public void testHasNetworkPermissionsAndConnectivityWhileRoaming() {
    ConnectivityManager connectivityManager = createMockConnectivityManager(true, true, true);
    mockPermissionGranted(Manifest.permission.ACCESS_NETWORK_STATE);
    when(context_.getSystemService(Context.CONNECTIVITY_SERVICE)).thenReturn(connectivityManager);
    assertFalse(NetworkUtil.hasNetworkPermissionsAndConnectivity(context_));
  }

  @Test
  public void testHasNetworkPermissionsAndConnectivityWithConnectivity() {
    ConnectivityManager connectivityManager = createMockConnectivityManager(true, true, false);
    mockPermissionGranted(Manifest.permission.ACCESS_NETWORK_STATE);
    when(context_.getSystemService(Context.CONNECTIVITY_SERVICE)).thenReturn(connectivityManager);
    assertTrue(NetworkUtil.hasNetworkPermissionsAndConnectivity(context_));
  }

  @Test
  public void testHasNetworkPermissionsAndConnectivityWithoutNetworkAccessPermission() {
    ConnectivityManager connectivityManager = createMockConnectivityManager(true, true, false);
    when(context_.checkCallingOrSelfPermission(Manifest.permission.ACCESS_NETWORK_STATE))
      .thenReturn(PackageManager.PERMISSION_DENIED);
    when(context_.getSystemService(Context.CONNECTIVITY_SERVICE)).thenReturn(connectivityManager);
    assertFalse(NetworkUtil.hasNetworkPermissionsAndConnectivity(context_));
  }

  private void mockPermissionGranted(String permission) {
    when(context_.checkCallingOrSelfPermission(permission))
      .thenReturn(PackageManager.PERMISSION_GRANTED);
  }

  private static ConnectivityManager createMockConnectivityManager(
    boolean isNetworkAvailable, boolean isConnected, boolean isRoaming) {
    ConnectivityManager connectivityManager = mock(ConnectivityManager.class);
    NetworkInfo networkInfo = mock(NetworkInfo.class);

    when(networkInfo.isAvailable()).thenReturn(isNetworkAvailable);
    when(networkInfo.isConnected()).thenReturn(isConnected);
    when(networkInfo.isRoaming()).thenReturn(isRoaming);
    when(connectivityManager.getActiveNetworkInfo()).thenReturn(networkInfo);
    return connectivityManager;
  }
}
