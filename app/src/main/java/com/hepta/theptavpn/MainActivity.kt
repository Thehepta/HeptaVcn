package com.hepta.theptavpn

import android.Manifest
import android.app.*

import android.content.ComponentName
import android.content.Intent
import android.content.ServiceConnection
import android.content.pm.PackageManager
import android.content.res.ColorStateList
import android.graphics.Color
import android.net.VpnService
import android.os.Build
import android.os.Bundle
import android.os.IBinder
import android.util.Log
import android.view.Menu
import android.view.MenuItem
import android.widget.Toast
import androidx.activity.result.ActivityResult
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.activity.viewModels
import androidx.appcompat.app.ActionBarDrawerToggle
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.app.NotificationCompat
import androidx.core.content.ContextCompat
import androidx.lifecycle.ViewModelProvider
import androidx.recyclerview.widget.LinearLayoutManager
import com.google.android.material.navigation.NavigationView
import com.hepta.theptavpn.LocalVPNService.ProxyBinder
import com.hepta.theptavpn.databinding.ActivityMainBinding


class MainActivity : AppCompatActivity(), NavigationView.OnNavigationItemSelectedListener {
    private var binding: ActivityMainBinding? = null
    var adapter: MainRecyclerAdapter? = null
    private var localVPNService: LocalVPNService? = null
    private var manager: NotificationManager? = null
    private var note: Notification? = null
    val mainViewModel: MainViewModel by viewModels()



    private val notificationRequestPermissionLauncher: ActivityResultLauncher<String> = registerForActivityResult(ActivityResultContracts.RequestPermission()){}


    private val startForResult = registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result: ActivityResult ->
        if (result.resultCode == Activity.RESULT_OK) {
            startVpnService()
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Log.e("Rzx","onCreate")
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding!!.root)
        title = getString(R.string.app_name)
        setSupportActionBar(binding!!.toolbar)
        binding!!.fab.setOnClickListener {
            if (mainViewModel!!.isRunning.value!!) {
                stopVpnService()
            } else {
                val vpnIntent = VpnService.prepare(this@MainActivity)
                if (vpnIntent != null){
                    startForResult.launch(vpnIntent)
                }else{
                    startVpnService()
                }
            }
        }
        binding!!.recyclerView.setHasFixedSize(true)
        binding!!.recyclerView.layoutManager = LinearLayoutManager(this)
        adapter = MainRecyclerAdapter(this)
        binding!!.recyclerView.adapter = adapter
        val toggle = ActionBarDrawerToggle(
            this,
            binding!!.drawerLayout,
            binding!!.toolbar,
            R.string.navigation_drawer_open,
            R.string.navigation_drawer_close
        )
        binding!!.drawerLayout.addDrawerListener(toggle)
        toggle.syncState()
        binding!!.navView.setNavigationItemSelectedListener(this)
        binding!!.version.text = BuildConfig.VERSION_NAME
        setupViewModel()
        createNotification()
//        requestPermission()
    }


    private fun requestPermission() {

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            if (lacksPermission(Manifest.permission.POST_NOTIFICATIONS)){
                if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.POST_NOTIFICATIONS)) {
                    deniedPermissionDialog("Notification Permission Required", " App need notification permission to enable this feature.")
                } else {
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        notificationRequestPermissionLauncher.launch(Manifest.permission.POST_NOTIFICATIONS)
                    }
                }
            }
        }
    }

    /**
     * 判断是否缺少权限
     */
    private fun lacksPermission( permission: String): Boolean {
        return ContextCompat.checkSelfPermission(applicationContext, permission) ==
                PackageManager.PERMISSION_DENIED
    }


    private fun deniedPermissionDialog(title: String, message: String) {
        val builder = AlertDialog.Builder(this)
        builder.setTitle(title)
            .setMessage(message)
            .setPositiveButton("OK") { dialogInterface, i -> dialogInterface.dismiss() }
        builder.create().show()
    }


    private fun createNotification() {
        manager = getSystemService(NOTIFICATION_SERVICE) as NotificationManager

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channel = NotificationChannel(
                getString(R.string.notify_channel_id), getString(R.string.notify_channel_name),
                NotificationManager.IMPORTANCE_DEFAULT
            )
            manager!!.createNotificationChannel(channel)
        }
        val intent = Intent(this, MainActivity::class.java)
        val pending = PendingIntent.getActivity(this, 0, intent, PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE)
        note = NotificationCompat.Builder(this, "ShadyPi")
            .setContentTitle("Thepta Application Capture Packet")
            .setContentText("running").setContentIntent(pending)
            .setSmallIcon(R.drawable.ic_launcher_foreground).setOngoing(true)
            .setColor(Color.parseColor("#ff0000")) //设置小图标颜色
            .setAutoCancel(false)
            .build()
    }


    private fun cancelNotification() {
        manager?.cancel(1);


    }
    private fun showNotification() {
        manager?.notify(1,note);

    }


    private fun setupViewModel() {
        mainViewModel!!.updateListAction.observe(this) { index ->
            if (index >= 0) {
                adapter!!.notifyItemChanged(index)
            } else {
                adapter!!.notifyDataSetChanged()
            }
        }
        mainViewModel!!.isRunning.observe(this) { isRunning ->
            adapter!!.isRunning = isRunning
            if (isRunning) {
                binding!!.fab.backgroundTintList = ColorStateList.valueOf(ContextCompat.getColor(this@MainActivity, R.color.colorSelected))
                showNotification()
            } else {
                binding!!.fab.backgroundTintList = ColorStateList.valueOf(ContextCompat.getColor(this@MainActivity, R.color.colorUnselected))
                cancelNotification()
            }
        }
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        menuInflater.inflate(R.menu.menu_main, menu)
        return super.onCreateOptionsMenu(menu)
    }

    override fun onResume() {
        super.onResume()
        mainViewModel!!.reloadServerList()
        if(localVPNService==null){
            val conn: ServiceConnection = object : ServiceConnection {
                override fun onServiceDisconnected(name: ComponentName)
                {
                    localVPNService = null
                }
                override fun onServiceConnected(name: ComponentName, service: IBinder)
                {
                    //返回一个MsgService对象
                    localVPNService = (service as ProxyBinder).getLocalVPNService(this@MainActivity)
                    mainViewModel.isRunning.value= localVPNService!!.Running
                }
            }
            val intent = Intent(applicationContext, LocalVPNService::class.java)
            bindService(intent, conn, BIND_AUTO_CREATE)
        }
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        when (item.itemId) {
            R.id.add_config -> {
                startActivity(Intent().setClass(this@MainActivity, ConfigActivity::class.java))
                return true
            }
        }
        return super.onOptionsItemSelected(item)
    }


    fun startVpnService() {
        val guid = mainViewModel!!.mainStorage.decodeString(MmkvManager.KEY_SELECTED_SERVER)
        if (guid != null) {
            localVPNService?.startVpnService(guid)

        } else {
            Toast.makeText(this, "select a server", Toast.LENGTH_LONG).show()
        }

    }

    fun stopVpnService() {
        localVPNService?.stopVpnService()

    }

    override fun onNavigationItemSelected(item: MenuItem): Boolean {
        when (item.itemId) {
            R.id.settings -> {
                startActivity(Intent(this, NSettingsActivity::class.java))
            }
        }
        return false
    }
}