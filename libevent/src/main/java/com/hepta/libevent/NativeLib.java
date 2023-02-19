package com.hepta.libevent;

public class NativeLib {

    // Used to load the 'libevent' library on application startup.
    static {
        System.loadLibrary("libevent");
    }

    /**
     * A native method that is implemented by the 'libevent' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}