package com.hepta.theptavpn;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.net.VpnService;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.hepta.theptavpn.databinding.ActivityMainBinding;

import java.util.List;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'theptavpn' library on application startup.

    private static final int VPN_REQUEST_CODE = 0x0F;

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
                Log.e("Rzx","stop");
                stopVpnService();
            }
        });
        binding.select.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent().setClass (MainActivity.this, appListActivity.class));
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
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == VPN_REQUEST_CODE && resultCode == RESULT_OK)
        {
            startVpnService();
        }


    }
    public void startVpnService() {
        Intent intent = new Intent(getApplicationContext(), LocalVPNService.class);
        String ServerAddress = mServerAddress.getText().toString();
        String ServerPort = mServerPort.getText().toString();
        intent.putExtra("serverAddress",ServerAddress);
        intent.putExtra("serverPort",ServerPort);
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