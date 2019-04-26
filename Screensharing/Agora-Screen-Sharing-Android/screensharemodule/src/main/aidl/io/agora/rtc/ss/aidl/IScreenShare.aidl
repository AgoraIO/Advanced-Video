// IScreenShare.aidl
package io.agora.rtc.ss.aidl;

import io.agora.rtc.ss.aidl.IErrorNotification;

// Declare any non-default types here with import statements

interface IScreenShare {
    void registerCallback(IErrorNotification callback);
    void unregisterCallback(IErrorNotification callback);
    void startShare();
    void stopShare();
}
