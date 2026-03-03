package com.the8thwall.c8.network;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.util.Log;

/**
 * Network utilities for android.
 */
public class NetworkUtil {

  private static final String TAG = "8thWall";

  /**
   * Determines whether the app has network permissions and a valid network connection is
   * available.
   */
  public static boolean hasNetworkPermissionsAndConnectivity(Context context) {
    // Double confirm that the network state permission is granted. If the project using this code didn't
    // properly merge our manifest with theirs, this should not crash.
    if (context.checkCallingOrSelfPermission(Manifest.permission.ACCESS_NETWORK_STATE)
        != PackageManager.PERMISSION_GRANTED) {
      Log.e(TAG, "Error: ACCESS_NETWORK_STATE permission is not granted.");
      return false;
    }

    ConnectivityManager connectivityManager =
    (ConnectivityManager)context.getSystemService(Context.CONNECTIVITY_SERVICE);
    NetworkInfo network = connectivityManager.getActiveNetworkInfo();
    return network != null && network.isAvailable() && network.isConnected()
      && !network.isRoaming();
  }
}
