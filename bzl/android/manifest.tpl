<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.the8thwall.bzl.hellobuild.android"
    android:versionCode="1"
    android:versionName="1.0" >

    <uses-sdk
        android:minSdkVersion="21"
        android:targetSdkVersion="21" />

    <application android:label="Hello Build" >
        <activity
            android:name="MainActivity"
            android:label="HelloBuild" >
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
        </activity>
    </application>
</manifest>
