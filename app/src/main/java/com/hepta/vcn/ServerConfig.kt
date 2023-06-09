package com.hepta.vcn

class ServerConfig {

    var remarks: String = ""
    var ipaddr: String = ""
    var port: Int = -1
    var netType: Int = -1
    var filterType: Int = -1

    companion object {

        fun create(): ServerConfig {

            return ServerConfig()
        }

        const val NET_TYPE_IPREF = 0
        const val NET_TYPE_HTTP = 1
        const val NET_TYPE_SOCKS = 2
    }

}
