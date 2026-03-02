package com.example.kotlinactivityapp

import android.os.Bundle
import android.app.Activity
import android.os.Build

class MainActivity : Activity() {
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.main)
    print(Build.VERSION_CODES.UPSIDE_DOWN_CAKE)
  }
}
