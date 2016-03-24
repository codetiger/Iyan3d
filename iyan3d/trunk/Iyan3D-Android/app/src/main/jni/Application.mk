APP_ABI := armeabi-v7a
APP_OPTIM := release
NDK_DEBUG=0
LOCAL_CFLAGS += -Os
APP_PLATFORM := android-14
APP_STL := c++_shared 
APP_CPPFLAGS += -DANDROID -frtti -DGL_RG8_EXT=0x822B -DGL_RED_EXT=0x1903 -std=c++11

