package com.hepta.theptavpn

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

    }

}
