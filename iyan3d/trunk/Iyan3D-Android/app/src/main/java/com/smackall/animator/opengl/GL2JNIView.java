package com.smackall.animator.opengl;

import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;

import com.smackall.animator.EditorView;
import com.smackall.animator.Helper.Constants;
import com.smackall.animator.Helper.PathManager;
import com.smackall.animator.Helper.SharedPreferenceManager;

import java.io.File;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;


public class GL2JNIView extends GLSurfaceView  {
    private static final boolean DEBUG = false;
    public static  boolean fileLoad=false;
    public static boolean  tapdetected=false;

    Renderer renderer;
    Context mContext;
    static SharedPreferenceManager sharedPreferenceManager;
    static Context staticContext;
    static boolean addVAOSupport = false;
    public GL2JNIView(Context context, SharedPreferenceManager sharedPreferenceManager) {
        super(context);
        mContext = context;
        GL2JNIView.sharedPreferenceManager = sharedPreferenceManager;
        this.staticContext = context;
        init(false, 0, 0);
    }

    public GL2JNIView(Context context, boolean translucent, int depth, int stencil) {
        super(context);
        init(translucent, depth, stencil);
    }

       private void init(boolean translucent, int depth, int stencil) {
        /* By default, GLSurfaceView() creates a RGB_565 opaque surface.
         * If we want a translucent one, we should change the surface's
         * format here, using PixelFormat.TRANSLUCENT for GL Surfaces
         * is interpreted as any 32-bit surface with alpha by SurfaceFlinger.
         */
        if (translucent) {
            this.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        }

        /* Setup the context factory for 2.0 rendering.
         * See ContextFactory class definition below
         */
           setEGLContextFactory(new ContextFactory());
        /* We need to choose an EGLConfig that matches the format of
         * our surface exactly. This is going to be done in our
         * custom config chooser. See ConfigChooser class definition
         * below.
         */
           setEGLConfigChooser(new ConfigChooser(8, 8, 8, 8, 24, 0));
        /* Set the renderer responsible for frame rendering */
        renderer = new Renderer(mContext);
        setRenderer(renderer);
        //setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }

    public static int detectOpenGLES() {
        ActivityManager am = (ActivityManager)staticContext.getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo info = am.getDeviceConfigurationInfo();
        return info.reqGlEsVersion;
    }


    private static class ContextFactory implements EGLContextFactory {
        private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
        public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig) {
            checkEglError("Before eglCreateContext", egl);

            EGLContext context;
            int[] openglesattrib = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL10.EGL_NONE };
            EGLConfig eglConfigCopy = eglConfig;
            EGLConfigChooser eglConfigChooser = null;

            if(detectOpenGLES() >= 0x30000) {
                addVAOSupport = true;
            } else {
                addVAOSupport = false;
                openglesattrib[1] = 2;
            }

            try {
                context = egl.eglCreateContext(display, eglConfigCopy, EGL10.EGL_NO_CONTEXT, openglesattrib);
            }
            catch (IllegalArgumentException ex) {
                eglConfigChooser = new ConfigChooser(5, 6, 5, 0, 16, 0);
                eglConfigCopy = eglConfigChooser.chooseConfig(egl, display);

                try {
                    context = egl.eglCreateContext(display, eglConfigCopy, EGL10.EGL_NO_CONTEXT, openglesattrib);
                }
                catch (IllegalArgumentException e) {
                    context = null;
                }
            }

            checkEglError("After eglCreateContext", egl);
            return context;
        }

        public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) {
            egl.eglDestroyContext(display, context);
        }
    }

    private static void checkEglError(String prompt, EGL10 egl) {
        int error;
        while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) {

        }
    }

    private static class ConfigChooser implements EGLConfigChooser {

        public ConfigChooser(int r, int g, int b, int a, int depth, int stencil) {
            mRedSize = r;
            mGreenSize = g;
            mBlueSize = b;
            mAlphaSize = a;
            mDepthSize = depth;
            mStencilSize = stencil;
        }

        /* This EGL config specification is used to specify 2.0 rendering.
         * We use a minimum size of 4 bits for red/green/blue, but will
         * perform actual matching in chooseConfig() below.
         */

        private static int EGL_OPENGL_ES2_BIT = 4;
        private static int[] s_configAttribs2 =
                {
                        EGL10.EGL_RED_SIZE, 8,
                        EGL10.EGL_GREEN_SIZE, 8,
                        EGL10.EGL_BLUE_SIZE, 8,
                        EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                        EGL10.EGL_NONE
                };

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {

            /* Get the number of minimally matching EGL configurations
             */
            int[] num_config = new int[1];
            egl.eglChooseConfig(display, s_configAttribs2, null, 0, num_config);

            int numConfigs = num_config[0];

            if (numConfigs <= 0) {
                throw new IllegalArgumentException("No configs match configSpec");
            }

            /* Allocate then read the array of minimally matching EGL configs
             */
            EGLConfig[] configs = new EGLConfig[numConfigs];
            egl.eglChooseConfig(display, s_configAttribs2, configs, numConfigs, num_config);

            if (DEBUG) {
                printConfigs(egl, display, configs);
            }
            /* Now return the "best" one
             */
            return chooseConfig(egl, display, configs);
        }

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display,
                                      EGLConfig[] configs) {
            for(EGLConfig config : configs) {
                int d = findConfigAttrib(egl, display, config,
                        EGL10.EGL_DEPTH_SIZE, 0);
                int s = findConfigAttrib(egl, display, config,
                        EGL10.EGL_STENCIL_SIZE, 0);

                // We need at least mDepthSize and mStencilSize bits
                if (d < mDepthSize || s < mStencilSize)
                    continue;

                // We want an *exact* match for red/green/blue/alpha
                int r = findConfigAttrib(egl, display, config,
                        EGL10.EGL_RED_SIZE, 0);
                int g = findConfigAttrib(egl, display, config,
                        EGL10.EGL_GREEN_SIZE, 0);
                int b = findConfigAttrib(egl, display, config,
                        EGL10.EGL_BLUE_SIZE, 0);
                int a = findConfigAttrib(egl, display, config,
                        EGL10.EGL_ALPHA_SIZE, 0);

                if (r == mRedSize && g == mGreenSize && b == mBlueSize && a == mAlphaSize)
                    return config;
            }
            return null;
        }

        private int findConfigAttrib(EGL10 egl, EGLDisplay display,
                                     EGLConfig config, int attribute, int defaultValue) {

            if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
                return mValue[0];
            }
            return defaultValue;
        }

        private void printConfigs(EGL10 egl, EGLDisplay display,
                                  EGLConfig[] configs) {
            int numConfigs = configs.length;
            for (int i = 0; i < numConfigs; i++) {
                printConfig(egl, display, configs[i]);
            }
        }

        private void printConfig(EGL10 egl, EGLDisplay display,
                                 EGLConfig config) {
            int[] attributes = {
                    EGL10.EGL_BUFFER_SIZE,
                    EGL10.EGL_ALPHA_SIZE,
                    EGL10.EGL_BLUE_SIZE,
                    EGL10.EGL_GREEN_SIZE,
                    EGL10.EGL_RED_SIZE,
                    EGL10.EGL_DEPTH_SIZE,
                    EGL10.EGL_STENCIL_SIZE,
                    EGL10.EGL_CONFIG_CAVEAT,
                    EGL10.EGL_CONFIG_ID,
                    EGL10.EGL_LEVEL,
                    EGL10.EGL_MAX_PBUFFER_HEIGHT,
                    EGL10.EGL_MAX_PBUFFER_PIXELS,
                    EGL10.EGL_MAX_PBUFFER_WIDTH,
                    EGL10.EGL_NATIVE_RENDERABLE,
                    EGL10.EGL_NATIVE_VISUAL_ID,
                    EGL10.EGL_NATIVE_VISUAL_TYPE,
                    0x3030, // EGL10.EGL_PRESERVED_RESOURCES,
                    EGL10.EGL_SAMPLES,
                    EGL10.EGL_SAMPLE_BUFFERS,
                    EGL10.EGL_SURFACE_TYPE,
                    EGL10.EGL_TRANSPARENT_TYPE,
                    EGL10.EGL_TRANSPARENT_RED_VALUE,
                    EGL10.EGL_TRANSPARENT_GREEN_VALUE,
                    EGL10.EGL_TRANSPARENT_BLUE_VALUE,
                    0x3039, // EGL10.EGL_BIND_TO_TEXTURE_RGB,
                    0x303A, // EGL10.EGL_BIND_TO_TEXTURE_RGBA,
                    0x303B, // EGL10.EGL_MIN_SWAP_INTERVAL,
                    0x303C, // EGL10.EGL_MAX_SWAP_INTERVAL,
                    EGL10.EGL_LUMINANCE_SIZE,
                    EGL10.EGL_ALPHA_MASK_SIZE,
                    EGL10.EGL_COLOR_BUFFER_TYPE,
                    EGL10.EGL_RENDERABLE_TYPE,
                    0x3042 // EGL10.EGL_CONFORMANT
            };
            String[] names = {
                    "EGL_BUFFER_SIZE",
                    "EGL_ALPHA_SIZE",
                    "EGL_BLUE_SIZE",
                    "EGL_GREEN_SIZE",
                    "EGL_RED_SIZE",
                    "EGL_DEPTH_SIZE",
                    "EGL_STENCIL_SIZE",
                    "EGL_CONFIG_CAVEAT",
                    "EGL_CONFIG_ID",
                    "EGL_LEVEL",
                    "EGL_MAX_PBUFFER_HEIGHT",
                    "EGL_MAX_PBUFFER_PIXELS",
                    "EGL_MAX_PBUFFER_WIDTH",
                    "EGL_NATIVE_RENDERABLE",
                    "EGL_NATIVE_VISUAL_ID",
                    "EGL_NATIVE_VISUAL_TYPE",
                    "EGL_PRESERVED_RESOURCES",
                    "EGL_SAMPLES",
                    "EGL_SAMPLE_BUFFERS",
                    "EGL_SURFACE_TYPE",
                    "EGL_TRANSPARENT_TYPE",
                    "EGL_TRANSPARENT_RED_VALUE",
                    "EGL_TRANSPARENT_GREEN_VALUE",
                    "EGL_TRANSPARENT_BLUE_VALUE",
                    "EGL_BIND_TO_TEXTURE_RGB",
                    "EGL_BIND_TO_TEXTURE_RGBA",
                    "EGL_MIN_SWAP_INTERVAL",
                    "EGL_MAX_SWAP_INTERVAL",
                    "EGL_LUMINANCE_SIZE",
                    "EGL_ALPHA_MASK_SIZE",
                    "EGL_COLOR_BUFFER_TYPE",
                    "EGL_RENDERABLE_TYPE",
                    "EGL_CONFORMANT"
            };
            int[] value = new int[1];
            for (int i = 0; i < attributes.length; i++) {
                int attribute = attributes[i];
                String name = names[i];
                if ( egl.eglGetConfigAttrib(display, config, attribute, value)) {
                } else {
                    // Log.w(TAG, String.format("  %s: failed\n", name));
                    while (egl.eglGetError() != EGL10.EGL_SUCCESS);
                }
            }
        }

        // Subclasses can adjust these values:
        protected int mRedSize;
        protected int mGreenSize;
        protected int mBlueSize;
        protected int mAlphaSize;
        protected int mDepthSize;
        protected int mStencilSize;
        private int[] mValue = new int[1];
    }

    private static class Renderer implements GLSurfaceView.Renderer {
        private Context mContext;

        public Renderer(Context mContext){
            this.mContext = mContext;
        }
        @Override
        public void onDrawFrame(GL10 gl) {
            if(!((EditorView)((Activity)mContext)).renderingPaused) {
                ((EditorView)((Activity)mContext)).renderManager.renderAll();
            }

        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {

        }

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            int maxUniform = GL2JNILib.init(Constants.width, Constants.height,addVAOSupport,sharedPreferenceManager.getInt(mContext,"maxUniform"));
            sharedPreferenceManager.setData(mContext,"maxUniform",maxUniform);
            loadFromFile();
        }

        public  void loadFromFile(){
            String filePath= PathManager.LocalProjectFolder+"/"+((EditorView)(Activity)mContext).projectNameHash+".sgb";
            File f = new File(filePath);
            if(f.exists()){
                GL2JNILib.loadScene(((EditorView)mContext).nativeCallBacks, filePath);
            }else{
                GL2JNILib.loadScene(((EditorView)mContext).nativeCallBacks, "init");
            }
        }
    }

    public static void callBackSurfaceRendered(){
        ((EditorView)((Activity)staticContext)).showOrHideLoading(Constants.HIDE);
        ((EditorView)((Activity)staticContext)).isDisplayPrepared = true;
        ((EditorView)((Activity)staticContext)).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(((EditorView) ((Activity) staticContext)).frameAdapter != null)
                    ((EditorView) ((Activity) staticContext)).frameAdapter.notifyDataSetChanged();

                if(Constants.isFirstTimeUser) {
                    ((EditorView) ((Activity) staticContext)).descriptionManager.helpForFirstTimeUser(staticContext);
                    ((EditorView) ((Activity) staticContext)).helpDialogs.showPop(staticContext);
                }
            }
        });
    }
}


