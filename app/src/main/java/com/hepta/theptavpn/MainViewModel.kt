package com.hepta.theptavpn

import android.app.Application
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.MutableLiveData
import com.hepta.theptavpn.MmkvManager.decodeServerList
import com.tencent.mmkv.MMKV

class MainViewModel(application: Application) : AndroidViewModel(application) {

    val mainStorage by lazy { MMKV.mmkvWithID(MmkvManager.ID_MAIN, MMKV.MULTI_PROCESS_MODE) }
    private val serverRawStorage by lazy { MMKV.mmkvWithID(MmkvManager.ID_SERVER_RAW, MMKV.MULTI_PROCESS_MODE) }

    var serverList = MmkvManager.decodeServerList()
    val serversCache = mutableListOf<ServersCache>()
    val updateListAction by lazy { MutableLiveData<Int>() }
    val isRunning by lazy { MutableLiveData<Boolean>() }



    override fun onCleared() {
        super.onCleared()
    }

    fun reloadServerList() {
        serverList = MmkvManager.decodeServerList()
        updateCache()
        updateListAction.value = -1
    }


    private fun updateCache() {
        serversCache.clear()
        for (guid in serverList) {
            val config = MmkvManager.decodeServerConfig(guid) ?: continue
            serversCache.add(ServersCache(guid, config))
        }
    }


    fun removeServer(guid: String) {
        serverList.remove(guid)
        MmkvManager.removeServer(guid)
        val index = getPosition(guid)
        if(index >= 0){
            serversCache.removeAt(index)
        }
    }

    fun getPosition(guid: String) : Int {
        serversCache.forEachIndexed { index, it ->
            if (it.guid == guid)
                return index
        }
        return -1
    }


}