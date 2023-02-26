package com.hepta.theptavpn;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.os.Bundle;
import android.os.PersistableBundle;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;

import com.hepta.theptavpn.databinding.ActivityApplistBinding;
import com.hepta.theptavpn.databinding.ActivityMainBinding;

import java.util.ArrayList;
import java.util.List;

public class appListActivity extends AppCompatActivity {


    private AppListAdapter appListAdapter;

    private ActivityApplistBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityApplistBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        initRecycleView();

    }

    private void initRecycleView() {
        List<AppListAdapter.AppInfo> appInfoList = getAllLauncherIconPackages(this);
        appListAdapter = new AppListAdapter(R.layout.item_application);
        LinearLayoutManager layoutManager = new LinearLayoutManager(this);
        layoutManager.setOrientation(LinearLayoutManager.VERTICAL);
        binding.recyclerView.setLayoutManager(layoutManager);
        binding.recyclerView.setAdapter(appListAdapter);
        appListAdapter.setList(appInfoList);
    }



    private List<AppListAdapter.AppInfo> getAllLauncherIconPackages(Context context) {
        List<AppListAdapter.AppInfo> launcherIconPackageList = new ArrayList<>();

        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_LAUNCHER);
        PackageManager mPm = getPackageManager();
        //set MATCH_ALL to prevent any filtering of the results
        List<ResolveInfo> resolveInfos = context.getPackageManager().queryIntentActivities(intent, PackageManager.MATCH_ALL);
        for (ResolveInfo info: resolveInfos) {
            AppListAdapter.AppInfo appInfo = new AppListAdapter.AppInfo(info.activityInfo.applicationInfo,mPm);
            launcherIconPackageList.add(appInfo);
        }

        return launcherIconPackageList;
    }
}
