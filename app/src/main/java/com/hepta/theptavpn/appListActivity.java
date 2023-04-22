package com.hepta.theptavpn;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.os.Bundle;
import android.os.PersistableBundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.listener.OnItemChildClickListener;
import com.hepta.theptavpn.databinding.ActivityApplistBinding;
import com.hepta.theptavpn.databinding.ActivityMainBinding;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class appListActivity extends BaseActivity {


    private AppListAdapter appListAdapter;

    private ActivityApplistBinding binding;
    List<AppListAdapter.AppInfo> launcherIconPackageList = new ArrayList<>();
    List<String> enableList = MmkvManager.INSTANCE.decodeApplicationList();
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityApplistBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        Objects.requireNonNull(getSupportActionBar()).setDisplayHomeAsUpEnabled(true);
        initRecycleView();

    }

    private void initRecycleView() {
        getAllLauncherIconPackages(this);
        appListAdapter = new AppListAdapter(R.layout.item_application);
        LinearLayoutManager layoutManager = new LinearLayoutManager(this);
        layoutManager.setOrientation(LinearLayoutManager.VERTICAL);
        binding.recyclerView.setLayoutManager(layoutManager);
        binding.recyclerView.setAdapter(appListAdapter);
        appListAdapter.setList(launcherIconPackageList);
        appListAdapter.setOnAppSelectListener(new AppListAdapter.onListener() {
            @Override
            public void OnListener(boolean status, String pKgName) {
                if(status){
                    enableList.add(pKgName);
                }else {
                    enableList.remove(pKgName);
                }

            }
        });


    }



    private List<AppListAdapter.AppInfo> getAllLauncherIconPackages(Context context) {

        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_LAUNCHER);
        PackageManager mPm = getPackageManager();
        //set MATCH_ALL to prevent any filtering of the results
        List<ResolveInfo> resolveInfos = context.getPackageManager().queryIntentActivities(intent, PackageManager.MATCH_ALL);
        for (ResolveInfo info: resolveInfos) {
            AppListAdapter.AppInfo appInfo = new AppListAdapter.AppInfo(info.activityInfo.applicationInfo,mPm);
            if(enableList.contains(appInfo.getPackageName())){
                appInfo.setEnable(true);
            }
            launcherIconPackageList.add(appInfo);
        }

        return launcherIconPackageList;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        boolean ret = super.onCreateOptionsMenu(menu);
        getMenuInflater().inflate(R.menu.applist_main_menu,menu);
        menu.findItem(R.id.ok).setOnMenuItemClickListener(new MenuItem.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(@NonNull MenuItem item) {
                MmkvManager.INSTANCE.encodeApplicationList(enableList);
                finish();
                return false;
            }
        });

        return ret;
    }
}
