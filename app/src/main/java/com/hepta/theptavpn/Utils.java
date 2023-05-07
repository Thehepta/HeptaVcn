package com.hepta.theptavpn;

import android.content.Context;

import java.io.BufferedReader;
import java.io.InputStreamReader;

public class Utils {

    public static int netmask2prefix(String netmask) {
        String[] data = netmask.split("\\.");
        int len = 0;
        for (String n : data) {
            len += (8 - Math.log(256-Integer.parseInt(n)) / Math.log(2));
        }
        return len;
    }

    public static String getCallStatck() {
        Throwable ex = new Throwable();
        StackTraceElement[] stackElements = ex.getStackTrace();
        String result = "";
        for (int i = 4; i < stackElements.length; i++) {
            result = result +"\n" +stackElements[i].getClassName()+"."+stackElements[i].getFileName()+":"+stackElements[i].getMethodName()+"("+stackElements[i].getLineNumber()+")";
        }
        return result;
    }

    public static void loge(String msg){

    }



    public static String getFromAssets(String fileName, Context context){
        try {
            InputStreamReader inputReader = new InputStreamReader(  context.getResources().getAssets().open(fileName) );
            BufferedReader bufReader = new BufferedReader(inputReader);
            String line="";
            StringBuilder Result= new StringBuilder();
            while((line = bufReader.readLine()) != null)
                Result.append(line);
            return Result.toString();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "";
    }
}
