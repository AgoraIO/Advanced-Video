package io.agora.advancedvideo.externvideosource;

interface IExternalVideoInputService {
    // the type of external video input is one of
    // ExternalVideoInputManager TYPE_LOCAL_VIDEO,
    // TYPE_SCREEN_SHARE
    // Bundle contains any information that is
    // necessary for this external video input
    void setExternalVideoInput(int type, in Intent intent);
}
