package io.agora.advancedvideo.externvideosource;

import android.opengl.EGLContext;

import io.agora.advancedvideo.gles.ProgramTextureOES;
import io.agora.advancedvideo.gles.core.EglCore;

class GLThreadContext {
    EglCore eglCore;
    EGLContext context;
    ProgramTextureOES program;
}
