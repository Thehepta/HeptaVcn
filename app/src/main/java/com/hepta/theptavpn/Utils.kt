package com.hepta.theptavpn

import android.text.Editable

object Utils {
    fun getEditable(text: String): Editable {
        return Editable.Factory.getInstance().newEditable(text)
    }
}