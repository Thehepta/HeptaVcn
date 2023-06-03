package com.hepta.vcn

import android.content.Intent
import android.graphics.Color
import android.text.TextUtils
import androidx.recyclerview.widget.RecyclerView
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.appcompat.app.AlertDialog
import com.hepta.vcn.databinding.ItemRecyclerMainBinding
import com.tencent.mmkv.MMKV

class MainRecyclerAdapter(val activity: MainActivity) : RecyclerView.Adapter<MainRecyclerAdapter.BaseViewHolder>() {
    companion object {
        private const val VIEW_TYPE_ITEM = 1
        private const val VIEW_TYPE_FOOTER = 2
    }

    private var mActivity: MainActivity = activity
    private val mainStorage by lazy { MMKV.mmkvWithID(MmkvManager.ID_MAIN, MMKV.MULTI_PROCESS_MODE) }


    var isRunning = false


    override fun getItemCount(): Int {
        return mActivity.mainViewModel?.serversCache?.size!!
    }


    override fun onBindViewHolder(holder: BaseViewHolder, position: Int) {
        if (holder is MainViewHolder) {
            val guid = mActivity.mainViewModel!!.serversCache[position].guid
            val config = mActivity.mainViewModel!!.serversCache[position].config


            holder.itemMainBinding.tvName.text = config.remarks
            holder.itemView.setBackgroundColor(Color.TRANSPARENT)

            holder.itemMainBinding.tvStatistics.text = "${config.ipaddr} : ${config.port}"

            if (guid == mainStorage?.decodeString(MmkvManager.KEY_SELECTED_SERVER)) {
                holder.itemMainBinding.layoutIndicator.setBackgroundResource(R.color.colorSelected)
            } else {
                holder.itemMainBinding.layoutIndicator.setBackgroundResource(R.color.colorUnselected)
            }


//            holder.itemMainBinding.layoutShare.setOnClickListener {
//                AlertDialog.Builder(mActivity).setItems(shareOptions.toTypedArray()) { _, i ->
//                    try {
//                        when (i) {
//                            0 -> {
//                                if (config.configType == EConfigType.CUSTOM) {
//                                    shareFullContent(guid)
//                                } else {
//                                    val ivBinding = ItemQrcodeBinding.inflate(LayoutInflater.from(mActivity))
//                                    ivBinding.ivQcode.setImageBitmap(AngConfigManager.share2QRCode(guid))
//                                    AlertDialog.Builder(mActivity).setView(ivBinding.root).show()
//                                }
//                            }
//                            1 -> {
//                                if (AngConfigManager.share2Clipboard(mActivity, guid) == 0) {
//                                    mActivity.toast(R.string.toast_success)
//                                } else {
//                                    mActivity.toast(R.string.toast_failure)
//                                }
//                            }
//                            2 -> shareFullContent(guid)
//                            else -> mActivity.toast("else")
//                        }
//                    } catch (e: Exception) {
//                        e.printStackTrace()
//                    }
//                }.show()
//            }

            holder.itemMainBinding.layoutEdit.setOnClickListener {
                val intent = Intent().putExtra("guid", guid).putExtra("isRunning", isRunning)
                mActivity.startActivity(intent.setClass(mActivity, ConfigActivity::class.java))

            }
            holder.itemMainBinding.layoutRemove.setOnClickListener {
                if (guid != mainStorage?.decodeString(MmkvManager.KEY_SELECTED_SERVER)) {
                        AlertDialog.Builder(mActivity).setMessage(R.string.del_config_comfirm)
                            .setPositiveButton(android.R.string.ok) { _, _ ->
                                removeServer(guid, position)
                            }
                            .show()
                }
            }

            holder.itemMainBinding.infoContainer.setOnClickListener {
                val selected = mainStorage?.decodeString(MmkvManager.KEY_SELECTED_SERVER)
                if (guid != selected) {
                    mainStorage?.encode(MmkvManager.KEY_SELECTED_SERVER, guid)
                    if (!TextUtils.isEmpty(selected)) {
                        notifyItemChanged(mActivity.mainViewModel!!.getPosition(selected!!))
                    }
                    notifyItemChanged(mActivity.mainViewModel!!.getPosition(guid))
//                    if (isRunning) {
//                        mActivity.showCircle()
//                        Utils.stopVService(mActivity)
//                        Observable.timer(500, TimeUnit.MILLISECONDS)
//                                .observeOn(AndroidSchedulers.mainThread())
//                                .subscribe {
//                                    V2RayServiceManager.startV2Ray(mActivity)
//                                    mActivity.hideCircle()
//                                }
//                    }
                }
            }
        }

    }

    private  fun removeServer(guid: String,position:Int) {
        mActivity.mainViewModel?.removeServer(guid)
        notifyItemRemoved(position)
        mActivity.mainViewModel?.serversCache?.let { notifyItemRangeChanged(position, it.size) }
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): BaseViewHolder {
        return MainViewHolder(ItemRecyclerMainBinding.inflate(LayoutInflater.from(parent.context), parent, false))
    }

    override fun getItemViewType(position: Int): Int {
        return if (position == mActivity.mainViewModel?.serversCache?.size) {
            VIEW_TYPE_FOOTER
        } else {
            VIEW_TYPE_ITEM
        }
    }

    open class BaseViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        fun onItemSelected() {
            itemView.setBackgroundColor(Color.LTGRAY)
        }

        fun onItemClear() {
            itemView.setBackgroundColor(0)
        }
    }

    class MainViewHolder(val itemMainBinding: ItemRecyclerMainBinding) :
            BaseViewHolder(itemMainBinding.root)

}
