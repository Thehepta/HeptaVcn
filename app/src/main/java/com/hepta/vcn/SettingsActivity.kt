package com.hepta.vcn

import android.content.Intent
import android.os.Bundle
import android.util.Log
import androidx.preference.*

class SettingsActivity : BaseActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_setting)
        title = getString(R.string.title_settings)
        supportActionBar!!.setDisplayHomeAsUpEnabled(true)
    }

    class SettingsFragment : PreferenceFragmentCompat() {
        private val proxy_none by lazy { findPreference<CheckBoxPreference>("pref_per_app_proxy_none") }

        private val proxy_allow by lazy { findPreference<CheckBoxPreference>("pref_per_app_proxy_allow") }
        private val proxy_allow_list by lazy { findPreference<Preference>("pref_per_app_proxy_allow_list") }

        private val proxy_disall by lazy { findPreference<CheckBoxPreference>("pref_per_app_proxy_disall") }
        private val proxy_disall_list by lazy { findPreference<Preference>("pref_per_app_proxy_disall_list") }

        private val vpn_addr by lazy { findPreference<EditTextPreference>("pref_vpn_addr") }
        private val vpn_netmask by lazy { findPreference<EditTextPreference>("pref_vpn_netmask") }
        private val vpn_mtu by lazy { findPreference<EditTextPreference>("pref_vpn_mtu") }

        private val type by lazy { MmkvManager.getAllowType() }

        override fun onCreatePreferences(savedInstanceState: Bundle?, rootKey: String?) {
            Log.e("Rzx","SettingsFragment onCreatePreferences")

            addPreferencesFromResource(R.xml.pref_settings)
            proxy_none?.setOnPreferenceChangeListener {_, any ->
                if(any as Boolean){
                    MmkvManager.setAllowType(MmkvManager.KEY_APP_ALLWO_NONE)
                    proxy_none?.isChecked=true
                    proxy_allow?.isChecked=false
                    proxy_allow_list?.isEnabled=false
                    proxy_disall?.isChecked=false
                    proxy_disall_list?.isEnabled=false

                    true
                }else{
                    false
                }
            }
            proxy_allow?.setOnPreferenceChangeListener {_, any ->
                if(any as Boolean){
                    MmkvManager.setAllowType(MmkvManager.KEY_APP_ADD_ALLOW)
                    proxy_none?.isChecked = false
                    proxy_allow?.isChecked = true
                    proxy_allow_list?.isEnabled=true
                    proxy_disall?.isChecked = false
                    proxy_disall_list?.isEnabled=false

                    true
                }else {
                    false
                }
            }



            proxy_disall?.setOnPreferenceChangeListener {_, any ->
                if(any as Boolean){
                    MmkvManager.setAllowType(MmkvManager.KEY_APP_ADD_DIS_ALLOW)
                    proxy_none?.isChecked = false
                    proxy_allow?.isChecked = false
                    proxy_allow_list?.isEnabled=false
                    proxy_disall?.isChecked = true
                    proxy_disall_list?.isEnabled=true

                    true
                }else {
                    false
                }
            }
            proxy_allow_list?.setOnPreferenceClickListener {
                startActivity(Intent(requireActivity(), appListActivity::class.java).putExtra("type",MmkvManager.KEY_APP_ADD_ALLOW))
                false
            }

            proxy_disall_list?.setOnPreferenceClickListener {
                startActivity(Intent(requireActivity(), appListActivity::class.java).putExtra("type",MmkvManager.KEY_APP_ADD_DIS_ALLOW))
                false
            }
            
            vpn_addr?.setOnPreferenceChangeListener { preference, newValue ->
                val value = newValue as String
                if(value.isNullOrBlank()){
                    false
                }else{
                    MmkvManager.setAddress(value)
                    preference.summary = value;
                    true
                }
            }
            vpn_netmask?.setOnPreferenceChangeListener { preference, newValue ->
                val value = newValue as String
                if(value.isNullOrBlank()){
                    false
                }else{
                    MmkvManager.setnetMask(value)
                    preference.summary = value;
                    true
                }
            }
            vpn_mtu?.setOnPreferenceChangeListener { preference, newValue ->
                val value = newValue as String
                if(value.isNullOrBlank()){
                    false
                }else{
                    MmkvManager.setMtu(value.toInt())
                    preference.summary = value;
                    true
                }
            }

            when(type){
                MmkvManager.KEY_APP_ALLWO_NONE->{
                    proxy_none?.isChecked=true
                    proxy_allow?.isChecked=false
                    proxy_allow_list?.isEnabled=false
                    proxy_disall?.isChecked=false
                    proxy_disall_list?.isEnabled=false
                }
                MmkvManager.KEY_APP_ADD_ALLOW->{
                    proxy_none?.isChecked = false
                    proxy_allow?.isChecked = true
                    proxy_allow_list?.isEnabled=true
                    proxy_disall?.isChecked = false
                    proxy_disall_list?.isEnabled=false
                }
                MmkvManager.KEY_APP_ADD_DIS_ALLOW->{
                    proxy_none?.isChecked = false
                    proxy_allow?.isChecked = false
                    proxy_allow_list?.isEnabled=false
                    proxy_disall?.isChecked = true
                    proxy_disall_list?.isEnabled=true
                }
            }
        }

        override fun onStart() {
            super.onStart()
            Log.e("Rzx","SettingsFragment onStart")
            vpn_mtu?.setSummary(MmkvManager.getMtu().toString())
            vpn_netmask?.setSummary(MmkvManager.getnetMask())
            vpn_addr?.setSummary(MmkvManager.getAddress())

        }
    }

    override fun onResume() {
        super.onResume()
    }
}