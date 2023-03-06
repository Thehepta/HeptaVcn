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

public class appListActivity extends AppCompatActivity {


    private AppListAdapter appListAdapter;

    private ActivityApplistBinding binding;
    List<AppListAdapter.AppInfo> launcherIconPackageList = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityApplistBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        setSupportActionBar(binding.toolbar);
        binding.toolbar.setNavigationIcon(R.drawable.toolbar_back);
//        binding.toolbar.getMenu().findItem()
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
                Intent intent = new Intent();
                intent.putExtra("data_return", "返回的数据");
                setResult(RESULT_OK, intent);
                finish();
                return false;
            }
        });
        menu.findItem(R.id.cancle).setOnMenuItemClickListener(new MenuItem.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(@NonNull MenuItem item) {
                onBackPressed();
                return false;
            }
        });
        return ret;
    }
}
