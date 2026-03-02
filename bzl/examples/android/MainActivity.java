package com.nianticlabs.bzl.examples.android;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import com.nianticlabs.bzl.examples.android.ExampleMethods;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        System.loadLibrary("hello-android");
        setContentView(R.layout.activity_main);
        TextView textView  = (TextView) findViewById(R.id.main_content);
        textView.setText(textView.getText() + "\n" + ExampleMethods.exampleString());
    }
}
