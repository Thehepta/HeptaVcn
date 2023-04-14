package com.hepta.theptavpn;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.net.VpnService;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.hepta.theptavpn.databinding.ActivityMainBinding;

import java.io.IOException;
import java.util.List;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'theptavpn' library on application startup.

    private static final int VPN_REQUEST_CODE = 0x0F;
    private static final int APPLIST_REQUEST_CODE = 0x1F;

    private ActivityMainBinding binding;
    private List<String> pkgNameList;
    private TextView mServerAddress;
    private TextView mServerPort;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        mServerAddress = binding.address;
        mServerPort = binding.port;
        // Example of a call to a native method



        binding.start.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startVPN();
            }
        });
        binding.stop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                Log.e("Rzx","stop");
                stopVpnService();
            }
        });
        binding.select.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivityForResult(new Intent().setClass (MainActivity.this, appListActivity.class),APPLIST_REQUEST_CODE);
            }
        });



        binding.curl.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                OkHttpClient client = new OkHttpClient();

                Request request = new Request.Builder()
                        .get()
                        .url("https://www.baidu.com")
                        .build();
                Call call = client.newCall(request);
                call.enqueue(new Callback() {
                    @Override
                    public void onFailure(Call call, IOException e) {
                        //失败情况
                        runOnUiThread(new Runnable() {//子线程内无法更新，通过runOnUiThread来更新界面Ui
                            @Override
                            public void run() {
                                Toast.makeText(MainActivity.this, "网络访问失败", Toast.LENGTH_SHORT).show();
                            }
                        });
                    }

                    @Override
                    public void onResponse(Call call, Response response) throws IOException {
                        //请求成功
                        response = client.newCall(request).execute();
                        String result = response.body().string();
                        runOnUiThread(new Runnable() {//子线程内无法更新，通过runOnUiThread来更新界面Ui
                            @Override
                            public void run() {
                                Toast.makeText(MainActivity.this, result, Toast.LENGTH_SHORT).show();
                            }
                        });
                    }
                });
            }
        });

    }


    private void startVPN()
    {
        Intent vpnIntent = VpnService.prepare(this);
        if (vpnIntent != null)
            startActivityForResult(vpnIntent, VPN_REQUEST_CODE);
        else
            onActivityResult(VPN_REQUEST_CODE, RESULT_OK, null);
    }


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        Log.e("Rzx","onActivityResult");

        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == VPN_REQUEST_CODE && resultCode == RESULT_OK)
        {
            startVpnService();
        }
        if (requestCode == APPLIST_REQUEST_CODE && resultCode == RESULT_OK){
            Log.e("Rzx","APPLIST_REQUEST_CODE");
        }



    }
    public void startVpnService() {

        Intent intent = new Intent(getApplicationContext(), LocalVPNService.class);
        String ServerAddress = mServerAddress.getText().toString();
        String ServerPort = mServerPort.getText().toString();
        intent.putExtra("serverAddress",ServerAddress);
        intent.putExtra("serverPort",ServerPort);
        if (binding.RadioDisable.isChecked()){
            intent.putExtra("appDisable",true);
        }else {
            intent.putExtra("appDisable",false);
        }
        startService(intent);
    }

    public void stopVpnService() {
        startService(new Intent(getApplicationContext(), LocalVPNService.class).setAction(LocalVPNService.ACTION_DISCONNECT));
    }

    /**
     * A native method that is implemented by the 'theptavpn' native library,
     * which is packaged with this application.
     */
}