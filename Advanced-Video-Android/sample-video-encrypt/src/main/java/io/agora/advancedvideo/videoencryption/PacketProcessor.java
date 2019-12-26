package io.agora.advancedvideo.videoencryption;

class PacketProcessor {
    static {
        System.loadLibrary("apm-plugin-packet-processing");
    }

    final void registerProcessing() {
        doRegisterProcessing();
    }

    final void unregisterProcessing() {
        doUnregisterProcessing();
    }

    private native void doRegisterProcessing();
    private native void doUnregisterProcessing();
}
