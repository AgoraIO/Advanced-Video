package io.agora.ex;

public class RTCPacketProcessing {
    static {
        System.loadLibrary("apm-plugin-packet-processing");
    }

    private native void doRegisterProcessing();

    public final void registerProcessing() {
        doRegisterProcessing();
    }

    private native void doUnregisterProcessing();

    public final void unregisterProcessing() {
        doUnregisterProcessing();
    }
}
