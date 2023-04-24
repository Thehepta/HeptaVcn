package com.hepta.theptavpn

import android.Manifest
import android.content.ComponentName
import android.content.Intent
import android.content.ServiceConnection
import android.content.res.ColorStateList
import android.net.Uri
import android.net.VpnService
import android.os.Build
import android.os.Bundle
import android.os.IBinder
import android.util.Log
import android.view.Menu
import android.view.MenuItem
import android.widget.Toast
import androidx.appcompat.app.ActionBarDrawerToggle
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.lifecycle.ViewModelProvider
import androidx.recyclerview.widget.LinearLayoutManager
import com.google.android.material.navigation.NavigationView
import com.hepta.theptavpn.LocalVPNService
import com.hepta.theptavpn.LocalVPNService.ProxyBinder
import com.hepta.theptavpn.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity(), NavigationView.OnNavigationItemSelectedListener {
    private var binding: ActivityMainBinding? = null
    var adapter: MainRecyclerAdapter? = null
    private var vpnServiceBinder: LocalVPNService? = null
    var mainViewModel: MainViewModel? = null
    private val notificationRequestPermissionLauncher = registerForActivityResult<String, Uri>(
        GetContent()
    ) {
        // Handle the returned Uri
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding!!.root)
        title = getString(R.string.app_name)
        setSupportActionBar(binding!!.toolbar)
        binding!!.fab.setOnClickListener {
            requestNotificationPermission()
            if (mainViewModel!!.isRunning.value!!) {
//                    binding.fabProgressCircle.show();
                stopVpnService()
                mainViewModel!!.isRunning.setValue(false)
            } else {
                mainViewModel!!.isRunning.setValue(true)
                startVpnService()
            }
        }
        mainViewModel = ViewModelProvider(this).get(MainViewModel::class.java)
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
        val vpnIntent = VpnService.prepare(this@MainActivity)
        if (vpnIntent != null) startActivityForResult(
            vpnIntent,
            VPN_REQUEST_CODE
        ) else onActivityResult(
            VPN_REQUEST_CODE, RESULT_OK, null
        )
    }

    private fun requestNotificationPermission() {
        if (ActivityCompat.shouldShowRequestPermissionRationale(
                this,
                Manifest.permission.POST_NOTIFICATIONS
            )
        ) {
            deniedPermissionDialog(
                "Notification Permission Required",
                " App need notification permission to enable this feature."
            )
        } else {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                notificationRequestPermissionLauncher.launch(Manifest.permission.POST_NOTIFICATIONS)
            }
        }
    }

    private fun deniedPermissionDialog(title: String, message: String) {
        val builder = AlertDialog.Builder(this)
        builder.setTitle(title)
            .setMessage(message)
            .setPositiveButton("OK") { dialogInterface, i -> dialogInterface.dismiss() }
        builder.create().show()
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
                binding!!.fab.backgroundTintList = ColorStateList.valueOf(
                    ContextCompat.getColor(
                        this@MainActivity,
                        R.color.colorSelected
                    )
                )
            } else {
                binding!!.fab.backgroundTintList = ColorStateList.valueOf(
                    ContextCompat.getColor(
                        this@MainActivity,
                        R.color.colorUnselected
                    )
                )
            }
        }
        mainViewModel!!.isRunning.value = false
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        menuInflater.inflate(R.menu.menu_main, menu)
        return super.onCreateOptionsMenu(menu)
    }

    override fun onResume() {
        super.onResume()
        mainViewModel!!.reloadServerList()
        Log.e("Rzx", "onResume")
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        when (item.itemId) {
            R.id.add_config -> {
                Log.e("Rzx", "add_config")
                startActivity(Intent().setClass(this@MainActivity, ConfigActivity::class.java))
                return true
            }
        }
        return super.onOptionsItemSelected(item)
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode == VPN_REQUEST_CODE && resultCode == RESULT_OK) {
            val conn: ServiceConnection = object : ServiceConnection {
                override fun onServiceDisconnected(name: ComponentName) {
                    vpnServiceBinder = null
                }

                override fun onServiceConnected(name: ComponentName, service: IBinder) {
                    //返回一个MsgService对象
                    vpnServiceBinder = (service as ProxyBinder).getMovieService(this@MainActivity)
                }
            }
            //            String guid = mainViewModel.getMainStorage().decodeString(MmkvManager.KEY_SELECTED_SERVER);
            val intent = Intent(applicationContext, LocalVPNService::class.java)
            bindService(intent, conn, BIND_AUTO_CREATE)
        }
    }

    fun startVpnService() {
        if (vpnServiceBinder == null) {
        } else {
            val guid = mainViewModel!!.mainStorage.decodeString(MmkvManager.KEY_SELECTED_SERVER)
            if (guid != null) {
                val run = vpnServiceBinder!!.startVpnService(guid)
                Log.e("rzx", "run:$run")
            } else {
                Toast.makeText(this, "select a server", Toast.LENGTH_LONG).show()
            }
        }
    }

    fun stopVpnService() {
        vpnServiceBinder!!.stopVpnService()
    }

    override fun onNavigationItemSelected(item: MenuItem): Boolean {
        when (item.itemId) {
            R.id.settings -> {
                startActivity(Intent(this, SettingsActivity::class.java))
            }
        }
        return false
    }

    companion object {
        private const val VPN_REQUEST_CODE = 0x0F
    }
}