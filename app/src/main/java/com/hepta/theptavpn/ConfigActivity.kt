package com.hepta.theptavpn

import android.os.Bundle
import android.text.Editable
import android.text.TextUtils
import android.util.Log
import android.view.Menu
import android.view.MenuItem

import com.hepta.theptavpn.MmkvManager.ID_MAIN
import com.hepta.theptavpn.MmkvManager.KEY_SELECTED_SERVER
import com.hepta.theptavpn.databinding.ActivityConfigBinding
import com.tencent.mmkv.MMKV

class ConfigActivity : BaseActivity() {

    private val mainStorage by lazy { MMKV.mmkvWithID(ID_MAIN, MMKV.MULTI_PROCESS_MODE) }
    private val editGuid by lazy { intent.getStringExtra("guid").orEmpty() }
    private val isRunning by lazy {
        intent.getBooleanExtra("isRunning", false)
                && editGuid.isNotEmpty()
                && editGuid == mainStorage?.decodeString(KEY_SELECTED_SERVER)
    }

    var binding: ActivityConfigBinding? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityConfigBinding.inflate(layoutInflater)
        setContentView(binding!!.root)

        val config = MmkvManager.decodeServerConfig(editGuid)
        config?.let {
            binding!!.etRemarks.text = Editable.Factory.getInstance().newEditable(it.remarks)
            binding!!.etAddress.text = Editable.Factory.getInstance().newEditable(it.ipaddr)
            binding!!.etPort.text = Editable.Factory.getInstance().newEditable(it.port.toString())
            binding!!.spNettype.setSelection(config.netType)
//            config.type = it.spNettype.selectedItemPosition
//            config. = it.spFilterApp.selectedItemPosition
        }

        supportActionBar?.setDisplayHomeAsUpEnabled(true)
//        binding?.spNettype?.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
//
//            override fun onItemSelected(p0: AdapterView<*>?, p1: View?, position: Int, id: Long) {
//                Nettype = position
//            }
//
//            override fun onNothingSelected(p0: AdapterView<*>?) {
//                TODO("Not yet implemented")
//            }
//        }

    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        menuInflater.inflate(R.menu.action_server, menu)
        val delButton = menu.findItem(R.id.del_config)
        val saveButton = menu.findItem(R.id.save_config)

        if (editGuid.isNotEmpty()) {
            if (isRunning) {
                delButton?.isVisible = false
                saveButton?.isVisible = false
            }
        } else {
            delButton?.isVisible = false
        }
        return super.onCreateOptionsMenu(menu)
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        // TODO Auto-generated method stub
        when (item.itemId) {
            android.R.id.home -> {
                finish()
                return true
            }
            R.id.del_config -> {
                deleteServer()
                return true
            }
            R.id.save_config -> {
                saveServer()
                return true
            }
            else -> {}
        }
        return super.onOptionsItemSelected(item)
    }

    private fun saveServer():Boolean {
        val config = MmkvManager.decodeServerConfig(editGuid) ?: ServerConfig.create()
        binding?.let{
            if (TextUtils.isEmpty(it.etAddress.text.toString())) {
//                toast(R.string.server_lab_address)
                return false
            }
            if (TextUtils.isEmpty(it.etPort.text.toString())) {
//                toast(R.string.server_lab_address)
                return false
            }
            if (TextUtils.isEmpty(it.etRemarks.text.toString())) {
//                toast(R.string.server_lab_address)
                return false
            }
            config.ipaddr = it.etAddress.text.toString()
            config.port = it.etPort.text.toString().toInt()
            config.remarks = it.etRemarks.text.toString()
            config.netType = it.spNettype.selectedItemPosition
        }

        MmkvManager.encodeServerConfig(editGuid, config)
//        toast(R.string.toast_success)
        finish()
        return true
    }

    private fun deleteServer() {
        finish()
    }
}