package com.hepta.theptavpn;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.res.ColorStateList;
import android.net.VpnService;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;
import androidx.recyclerview.widget.LinearLayoutManager;

import com.hepta.theptavpn.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding binding;
    MainRecyclerAdapter adapter ;
    private static final int VPN_REQUEST_CODE = 0x0F;
    private LocalVPNService vpnServiceBinder = null;

    public MainViewModel mainViewModel;




    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        setTitle(getString(R.string.app_name));
        setSupportActionBar(binding.toolbar);
        binding.fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mainViewModel.isRunning().getValue()) {
//                    binding.fabProgressCircle.show();
                    stopVpnService();
                    mainViewModel.isRunning().setValue(false);
                }else {
                    mainViewModel.isRunning().setValue(true);

                    startVpnService();

                }
            }
        });

        mainViewModel = new ViewModelProvider(this).get(MainViewModel.class);

        binding.recyclerView.setHasFixedSize(true);
        binding.recyclerView.setLayoutManager(new LinearLayoutManager(this));
        adapter = new MainRecyclerAdapter(this);
        binding.recyclerView.setAdapter(adapter);

        setupViewModel();
        Intent vpnIntent = VpnService.prepare(MainActivity.this);
        if (vpnIntent != null)
            startActivityForResult(vpnIntent, VPN_REQUEST_CODE);
        else
            onActivityResult(VPN_REQUEST_CODE, RESULT_OK, null);

    }

    private void setupViewModel() {
        mainViewModel.getUpdateListAction().observe(this, new Observer<Integer>() {
            @Override
            public void onChanged(Integer index) {
                if (index >= 0) {
                    adapter.notifyItemChanged(index);
                } else {
                    adapter.notifyDataSetChanged();
                }
            }
        });

        mainViewModel.isRunning().observe(this, new Observer<Boolean>() {
            @Override
            public void onChanged(Boolean isRunning) {
                adapter.setRunning(isRunning);
                if (isRunning) {
                    binding.fab.setBackgroundTintList( ColorStateList.valueOf(ContextCompat.getColor(MainActivity.this, R.color.colorSelected)));
                }else {
                    binding.fab.setBackgroundTintList( ColorStateList.valueOf(ContextCompat.getColor(MainActivity.this, R.color.colorUnselected)));
                }

            }
        });
        mainViewModel.isRunning().setValue(false);

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    protected void onResume() {
        super.onResume();
        mainViewModel.reloadServerList();

        Log.e("Rzx","onResume");
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        switch (item.getItemId()){
            case R.id.filter_config:
                Log.e("Rzx","filter_config");
                return true;
            case R.id.add_config:
                Log.e("Rzx","add_config");
                startActivity(new Intent().setClass (MainActivity.this, ConfigActivity.class));
                return true;
        }

        return super.onOptionsItemSelected(item);
    }



    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {

        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == VPN_REQUEST_CODE && resultCode == RESULT_OK)
        {
            ServiceConnection conn = new ServiceConnection() {
                @Override
                public void onServiceDisconnected(ComponentName name) {
                    vpnServiceBinder = null;
                }
                @Override
                public void onServiceConnected(ComponentName name, IBinder service) {
                    //返回一个MsgService对象
                    vpnServiceBinder = ((LocalVPNService.ProxyBinder)service).getMovieService(MainActivity.this);
                }
            };
//            String guid = mainViewModel.getMainStorage().decodeString(MmkvManager.KEY_SELECTED_SERVER);
            Intent intent = new Intent(getApplicationContext(), LocalVPNService.class);
            bindService(intent, conn, Context.BIND_AUTO_CREATE);
        }
    }
    public void startVpnService(){

        if (vpnServiceBinder ==null){

        }else {
            String guid = mainViewModel.getMainStorage().decodeString(MmkvManager.KEY_SELECTED_SERVER);
            if(guid != null){
                boolean run = vpnServiceBinder.startVpnService(guid);
                Log.e("rzx","run:"+run);
            }else {
                Toast.makeText(this,"select a server",Toast.LENGTH_LONG).show();
            }

        }

//        engine.Key key = new Key();
//        key.setMark(0);
//        key.setMTU(0);
//        key.setDevice("fd://" + tunDevice.getFd()); // <--- here
//        key.setInterface("");
//        key.setLogLevel("debug");
//        key.setProxy("socks5://127.0.0.1:1080"); // <--- and here
//        key.setRestAPI("");
//        key.setTCPSendBufferSize("");
//        key.setTCPReceiveBufferSize("");
//        key.setTCPModerateReceiveBuffer(false);
//
//        engine.Engine.insert(key);
//        engine.Engine.start();



    }
    public void stopVpnService() {
        vpnServiceBinder.stopVpnService();
    }

}
