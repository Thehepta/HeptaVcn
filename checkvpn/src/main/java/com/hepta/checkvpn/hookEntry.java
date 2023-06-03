package com.hepta.checkvpn;

import android.content.Context;
import android.util.Log;

public class hookEntry {

    static {
        System.loadLibrary("hookvpn");
    }

    public  native void inject();

    public  void Entry(Context context){

        Log.e("pine hook1","start hookvpn");
        inject();

    }



}
