package com.hepta.ipvpn;
import android.os.Parcel;

public class PptpProfile {
    private static final long serialVersionUID = 1L;
    private boolean mEncryption = true;

    @Override
    public VpnType getType() {
        return VpnType.PPTP;
    }

    /**
     * Enables/disables the encryption for PPTP tunnel.
     */
    public void setEncryptionEnabled(boolean enabled) {
        mEncryption = enabled;
    }

    public boolean isEncryptionEnabled() {
        return mEncryption;
    }

    @Override
    protected void readFromParcel(Parcel in) {
        super.readFromParcel(in);
        mEncryption = in.readInt() > 0;
    }

    @Override
    public void writeToParcel(Parcel parcel, int flags) {
        super.writeToParcel(parcel, flags);
        parcel.writeInt(mEncryption ? 1 : 0);
    }
}