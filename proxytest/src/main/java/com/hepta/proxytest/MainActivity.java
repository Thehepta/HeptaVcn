package com.hepta.proxytest;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.io.IOException;
import java.math.BigInteger;
import java.security.KeyStore;
import java.security.PublicKey;
import java.security.cert.X509Certificate;
import java.util.Enumeration;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button button = findViewById(R.id.proxyhttps);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
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


        Button proxyhttp = findViewById(R.id.proxyhttp);
        proxyhttp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                OkHttpClient client = new OkHttpClient();

                Request request = new Request.Builder()
                        .get()
                        .url("http://www.baidu.com")
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


        Button travecert = findViewById(R.id.travecert);
        travecert.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                try {
                    KeyStore keyStore = KeyStore.getInstance("AndroidCAStore");
                    keyStore.load(null, null);
                    Enumeration<String> aliases = keyStore.aliases();
                    while (aliases.hasMoreElements()) {
                        String alias = aliases.nextElement();
                        X509Certificate cert = (X509Certificate) keyStore.getCertificate(alias);
                        if (isSelfSigned(cert)) {
                            System.out.println("Certificate is self-signed."+cert.getSubjectDN());
                        } else {
                            System.out.println("Certificate is not self-signed.");
                        }
                        // Process the certificate as needed
                        System.out.println("Certificate: " + cert.getSubjectDN());
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });

    }

    public static boolean isSelfSigned(X509Certificate cert) {
        PublicKey publicKey = cert.getPublicKey();
        try {
            cert.verify(publicKey);
            // If verification is successful and subject is equal to issuer, it's a root or self-signed certificate
            return cert.getSubjectDN().equals(cert.getIssuerDN());
        } catch (Exception e) {
            return false; // Verification failed, not a root or self-signed certificate
        }
    }
}