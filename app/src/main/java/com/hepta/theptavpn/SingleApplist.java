package com.hepta.theptavpn;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;

import java.util.ArrayList;
import java.util.List;

public class SingleApplist {

    private static SingleApplist _Instance;

    public static SingleApplist getInstance(){
        if (_Instance == null){
            _Instance = new SingleApplist();
        }
        return _Instance;
    }

    private SingleApplist(){

//        getAllLauncherIconPackages();
    }

    private List<AppListAdapter.AppInfo> getAllLauncherIconPackages(Context context) {
        List<AppListAdapter.AppInfo> launcherIconPackageList = new ArrayList<>();

        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_LAUNCHER);
        PackageManager mPm = context.getPackageManager();
        //set MATCH_ALL to prevent any filtering of the results
        List<ResolveInfo> resolveInfos = context.getPackageManager().queryIntentActivities(intent, PackageManager.MATCH_ALL);
        for (ResolveInfo info: resolveInfos) {
            AppListAdapter.AppInfo appInfo = new AppListAdapter.AppInfo(info.activityInfo.applicationInfo,mPm);
            launcherIconPackageList.add(appInfo);
        }

        return launcherIconPackageList;
    }
}
