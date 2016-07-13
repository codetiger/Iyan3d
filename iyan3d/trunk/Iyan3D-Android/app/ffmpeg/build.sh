#!/bin/bash
###############################################################################
#
# This script will build FFMPEG for android.
#
# Prerequisits:
# - FFMPEG source checked out / copied to src subfolder.
#
# Build steps:
# - Patch the FFMPEG configure script to fix missing support for shared
# library versioning on android.
# - Configure FFMPEG
# - Build FFMPEG
#
###############################################################################
export ANDROID_NDK=/storage/android-developer/Android/Sdk/ndk-bundle
SCRIPT=$(readlink -f $0)
BASE=$(dirname $SCRIPT)
###############################################################################
#
# Argument parsing.
# Allow some components to be overwritten by command line arguments.
#
###############################################################################
if [ -z $HOST_ARCH]; then
HOST_ARCH=$(uname -m)
fi
if [ -z $PLATFORM ]; then
PLATFORM=14
fi
if [ -z $MAKE_OPTS ]; then
MAKE_OPTS="-j4"
fi
function usage
{
echo "$0 [-a <ndk>] [-h <host arch>] [-m <make opts>] [-p <android platform>]"
echo -e "\tdefaults:"
echo -e "\tHOST_ARCH=$HOST_ARCH"
echo -e "\tPLATFORM=$PLATFORM"
echo -e "\tMAKE_OPTS=$MAKE_OPTS"
echo -e "\tANDROID_NDK must be set manually."
echo ""
echo -e "\tAll arguments can also be set as environment variables."
exit -3
}
while getopts "a:h:m:p:" opt; do
case $opt in
a)
ANDROID_NDK=$OPTARG
;;
h)
HOST_ARCH=$OPTARG
;;
m)
MAKE_OPTS=$OPTARG
;;
p)
PLATFORM=$OPTARG
;;
\?)
echo "Invalid option $OPTARG" >&2
usage
;;
esac
done
if [ -z $HOST_ARCH ]; then
HOST_ARCH=$(uname -m)
fi
if [ -z $PLATFORM ]; then
PLATFORM=9
fi
if [ -z $MAKE_OPTS ]; then
MAKE_OPTS="-j4"
fi
if [ -z $ANDROID_NDK ]; then
echo "ANDROID_NDK not set. Set it to the directory of your NDK installation."
exit -1
fi
if [ ! -d $BASE/src ]; then
echo "Please copy or check out FFMPEG source to folder src!"
exit -2
fi
echo "Building with:"
echo "HOST_ARCH=$HOST_ARCH"
echo "PLATFORM=$PLATFORM"
echo "MAKE_OPTS=$MAKE_OPTS"
echo "ANDROID_NDK=$ANDROID_NDK"
cd $BASE/src
# Save original configuration file
# or restore original before applying patches.
if [ ! -f configure.bak ]; then
echo "Saving original configure file to configure.bak"
cp configure configure.bak
else
echo "Restoring original configure file from configure.bak"
cp configure.bak configure
fi
patch -p1 < $BASE/patches/config.patch
if [ ! -f library.mak.bak ]; then
echo "Saving original library.mak file to library.mak.bak"
cp library.mak library.mak.bak
else
echo "Restoring original library.mak file from library.mak.bak"
cp library.mak.bak library.mak
fi
patch -p1 < $BASE/patches/library.mak.patch
# Remove old build and installation files.
if [ -d $BASE/install ]; then
rm -rf $BASE/install
fi
if [ -d $BASE/build ]; then
rm -rf $BASE/build
fi
###############################################################################
#
# build_one ... builds FFMPEG with provided arguments.
#
# Calling convention:
#
# build_one <PREFIX> <CROSS_PREFIX> <ARCH> <SYSROOT> <CFLAGS> <LDFLAGS> <EXTRA>
#
# PREFIX ... Installation directory
# CROSS_PREFIX ... Full path with toolchain prefix
# ARCH ... Architecture to build for (arm, x86, mips)
# SYSROOT ... Android platform to build for, full path.
# CFLAGS ... Additional CFLAGS for building.
# LDFLAGS ... Additional LDFLAGS for linking
# EXTRA ... Any additional configuration flags, e.g. --cpu=XXX
#
###############################################################################
function build_one
{
mkdir -p $1
cd $1
$BASE/src/configure \
--prefix=$2 \
--enable-shared \
--enable-static \
--disable-doc \
--disable-ffmpeg \
--disable-ffplay \
--disable-ffprobe \
--disable-ffserver \
--disable-avdevice \
--disable-doc \
--enable-symver \
--enable-decoder=h264 \
--enable-parser=h264 \
--enable-decoder=h263 \
--enable-runtime-cpudetect \
--cross-prefix=$3 \
--target-os=linux \
--enable-pic \
--arch=$4 \
--enable-cross-compile \
--sysroot=$5 \
--extra-cflags="-Os $6" \
--extra-ldflags="$7" \
$8
make clean
make $MAKE_OPTS
make install
}
NDK=$ANDROID_NDK
###############################################################################
#
# ARM build configuration
#
###############################################################################
PREFIX=$BASE/install/armeabi
BUILD_ROOT=$BASE/build/armeabi
PLATFORM=14
SYSROOT=$NDK/platforms/android-$PLATFORM/arch-arm/
TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-$HOST_ARCH
CROSS_PREFIX=$TOOLCHAIN/bin/arm-linux-androideabi-
ARCH=arm
E_CFLAGS=
E_LDFLAGS=
EXTRA=
build_one "$BUILD_ROOT" "$PREFIX" "$CROSS_PREFIX" "$ARCH" "$SYSROOT" \
"$E_CFLAGS" "$E_LDFLAGS" "$EXTRA"
TOOLCHAIN=/home/android-developer/Downloads/ffmpeg-3.1.1/toolchain
$ANDROID_NDK/build/tools/make-standalone-toolchain.sh --toolchain=arm-linux-androideabi-4.9 --arch=arm --platform=android-14 --install-dir=$TOOLCHAIN
export PATH=$TOOLCHAIN/bin:$PATH
export CC="ccache arm-linux-androideabi-gcc"
export LD=arm-linux-androideabi-ld
export AR=arm-linux-androideab-ar
rm $BUILD_ROOT/libavcodec/log2_tab.o $BUILD_ROOT/libavformat/log2_tab.o $BUILD_ROOT/libswresample/log2_tab.o $BUILD_ROOT/libavfilter/log2_tab.o $BUILD_ROOT/libavutil/reverse.o $BUILD_ROOT/libavutil/log2_tab.o
$CC -lm -lz -shared --sysroot=$SYSROOT -Wl,--no-undefined -Wl,-z,noexecstack $E_LDFLAGS $BUILD_ROOT/libswresample/*.o $BUILD_ROOT/libswresample/arm/*.o \
$BUILD_ROOT/libswscale/*.o $BUILD_ROOT/libswscale/arm/*.o $BUILD_ROOT/libavfilter/*.o $BUILD_ROOT/libavformat/*.o \
$BUILD_ROOT/libavcodec/*.o $BUILD_ROOT/libavcodec/arm/*.o $BUILD_ROOT/compat/*.o $BUILD_ROOT/libavutil/*.o $BUILD_ROOT/libavutil/arm/*.o \
-o /home/android-developer/Downloads/ffmpeg-3.1.1/install/armeabi/lib/libffmpeg.so
###############################################################################
#
# ARM-v7a build configuration
#
###############################################################################
PREFIX=$BASE/install/armeabi-v7a
BUILD_ROOT=$BASE/build/armeabi-v7a
PLATFORM=14
SYSROOT=$NDK/platforms/android-$PLATFORM/arch-arm/
TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-$HOST_ARCH
CROSS_PREFIX=$TOOLCHAIN/bin/arm-linux-androideabi-
ARCH=arm
E_CFLAGS="-march=armv7-a -mfloat-abi=softfp"
E_LDFLAGS=
EXTRA=
build_one "$BUILD_ROOT" "$PREFIX" "$CROSS_PREFIX" "$ARCH" "$SYSROOT" \
"$E_CFLAGS" "$E_LDFLAGS" "$EXTRA"

TOOLCHAIN=/home/android-developer/Downloads/ffmpeg-3.1.1/toolchain
$ANDROID_NDK/build/tools/make-standalone-toolchain.sh --toolchain=arm-linux-androideabi-4.9 --arch=arm --platform=android-14 --install-dir=$TOOLCHAIN
export PATH=$TOOLCHAIN/bin:$PATH
export CC="ccache arm-linux-androideabi-gcc"
export LD=arm-linux-androideabi-ld
export AR=arm-linux-androideab-ar
rm $BUILD_ROOT/libavcodec/log2_tab.o $BUILD_ROOT/libavformat/log2_tab.o $BUILD_ROOT/libswresample/log2_tab.o $BUILD_ROOT/libavfilter/log2_tab.o $BUILD_ROOT/libavutil/reverse.o $BUILD_ROOT/libavutil/log2_tab.o
$CC -lm -lz -shared --sysroot=$SYSROOT -Wl,--no-undefined -Wl,-z,noexecstack $E_LDFLAGS $BUILD_ROOT/libswresample/*.o $BUILD_ROOT/libswresample/arm/*.o \
$BUILD_ROOT/libswscale/*.o $BUILD_ROOT/libswscale/arm/*.o $BUILD_ROOT/libavfilter/*.o $BUILD_ROOT/libavformat/*.o \
$BUILD_ROOT/libavcodec/*.o $BUILD_ROOT/libavcodec/arm/*.o $BUILD_ROOT/compat/*.o $BUILD_ROOT/libavutil/*.o $BUILD_ROOT/libavutil/arm/*.o \
-o $PREFIX/lib/libffmpeg.so

###############################################################################
#
# x86 build configuration
#
###############################################################################
PREFIX=$BASE/install/x86
BUILD_ROOT=$BASE/build/x86
PLATFORM=14
SYSROOT=$NDK/platforms/android-$PLATFORM/arch-x86/
TOOLCHAIN=$NDK/toolchains/x86-4.9/prebuilt/linux-$HOST_ARCH
CROSS_PREFIX=$TOOLCHAIN/bin/i686-linux-android-
ARCH=x86
E_CFLAGS="-march=i686 -mtune=intel -mssse3 -mfpmath=sse -m32"
E_LDFLAGS=
EXTRA="--enable-asm"
build_one "$BUILD_ROOT" "$PREFIX" "$CROSS_PREFIX" "$ARCH" "$SYSROOT" \
"$E_CFLAGS" "$E_LDFLAGS" "$EXTRA"

TOOLCHAIN=/home/android-developer/Downloads/ffmpeg-3.1.1/toolchain
$ANDROID_NDK/build/tools/make-standalone-toolchain.sh --toolchain=x86-4.9 --arch=x86 --platform=android-14 --install-dir=$TOOLCHAIN
export PATH=$TOOLCHAIN/bin:$PATH
export CC="ccache i686-linux-android-gcc"
export LD=i686-linux-android-ld
export AR=i686-linux-android-ar
rm $BUILD_ROOT/libavcodec/log2_tab.o $BUILD_ROOT/libavformat/log2_tab.o $BUILD_ROOT/libswresample/log2_tab.o $BUILD_ROOT/libavfilter/log2_tab.o $BUILD_ROOT/libavutil/reverse.o $BUILD_ROOT/libavutil/log2_tab.o
$CC -lm -lz -shared --sysroot=$SYSROOT -Wl,--no-undefined -Wl,-z,noexecstack $E_LDFLAGS $BUILD_ROOT/libswresample/*.o $BUILD_ROOT/libswresample/x86/*.o \
$BUILD_ROOT/libswscale/*.o $BUILD_ROOT/libswscale/x86/*.o $BUILD_ROOT/libavfilter/*.o $BUILD_ROOT/libavformat/*.o \
$BUILD_ROOT/libavcodec/*.o $BUILD_ROOT/libavcodec/x86/*.o $BUILD_ROOT/compat/*.o $BUILD_ROOT/libavutil/*.o $BUILD_ROOT/libavutil/x86/*.o \
-o $PREFIX/lib/libffmpeg.so
###############################################################################
#
# ARM64 build configuration
#
###############################################################################
PREFIX=$BASE/install/arm64-v8a
BUILD_ROOT=$BASE/build/arm64-v8a
PLATFORM=21
SYSROOT=$NDK/platforms/android-$PLATFORM/arch-arm64/
TOOLCHAIN=$NDK/toolchains/aarch64-linux-android-4.9/prebuilt/linux-$HOST_ARCH
CROSS_PREFIX=$TOOLCHAIN/bin/aarch64-linux-android-
ARCH=arm64
E_CFLAGS=
E_LDFLAGS=
EXTRA=
build_one "$BUILD_ROOT" "$PREFIX" "$CROSS_PREFIX" "$ARCH" "$SYSROOT" \
"$E_CFLAGS" "$E_LDFLAGS" "$EXTRA"

TOOLCHAIN=/home/android-developer/Downloads/ffmpeg-3.1.1/toolchain
$ANDROID_NDK/build/tools/make-standalone-toolchain.sh --toolchain=aarch64-linux-android-4.9 --arch=arm64 --platform=android-21 --install-dir=$TOOLCHAIN
export PATH=$TOOLCHAIN/bin:$PATH
export CC="ccache aarch64-linux-android-gcc"
export LD=aarch64-linux-android-ld
export AR=aarch64-linux-android-ar
rm $BUILD_ROOT/libavcodec/log2_tab.o $BUILD_ROOT/libavformat/log2_tab.o $BUILD_ROOT/libswresample/log2_tab.o $BUILD_ROOT/libavfilter/log2_tab.o $BUILD_ROOT/libavutil/reverse.o $BUILD_ROOT/libavutil/log2_tab.o
$CC -lm -lz -shared --sysroot=$SYSROOT -Wl,--no-undefined -Wl,-z,noexecstack $E_LDFLAGS $BUILD_ROOT/libswresample/*.o $BUILD_ROOT/libswresample/aarch64/*.o \
$BUILD_ROOT/libswscale/*.o $BUILD_ROOT/libswscale/aarch64/*.o $BUILD_ROOT/libavfilter/*.o $BUILD_ROOT/libavformat/*.o \
$BUILD_ROOT/libavcodec/*.o $BUILD_ROOT/libavcodec/aarch64/*.o $BUILD_ROOT/libavcodec/neon/*.o $BUILD_ROOT/compat/*.o $BUILD_ROOT/libavutil/*.o $BUILD_ROOT/libavutil/aarch64/*.o \
-o $PREFIX/lib/libffmpeg.so
###############################################################################
#
# X86_64 build configuration
#
###############################################################################
PREFIX=$BASE/install/x86_64
BUILD_ROOT=$BASE/build/x86_64
PLATFORM=21
SYSROOT=$NDK/platforms/android-$PLATFORM/arch-x86_64/
TOOLCHAIN=$NDK/toolchains/x86_64-4.9/prebuilt/linux-$HOST_ARCH
CROSS_PREFIX=$TOOLCHAIN/bin/x86_64-linux-android-
ARCH=x86_64
E_CFLAGS="-march=x86-64 -msse4.2 -mpopcnt -m64 -mtune=intel"
E_LDFLAGS=
EXTRA="--enable-asm"
build_one "$BUILD_ROOT" "$PREFIX" "$CROSS_PREFIX" "$ARCH" "$SYSROOT" \
"$E_CFLAGS" "$E_LDFLAGS" "$EXTRA"

TOOLCHAIN=/home/android-developer/Downloads/ffmpeg-3.1.1/toolchain
$ANDROID_NDK/build/tools/make-standalone-toolchain.sh --toolchain=x86_64-4.9 --arch=x86_64 --platform=android-21 --install-dir=$TOOLCHAIN
export PATH=$TOOLCHAIN/bin:$PATH
export CC="ccache x86_64-linux-android-gcc"
export LD=x86_64-linux-android-ld
export AR=x86_64-linux-android-ar
rm $BUILD_ROOT/libavcodec/log2_tab.o $BUILD_ROOT/libavformat/log2_tab.o $BUILD_ROOT/libswresample/log2_tab.o $BUILD_ROOT/libavfilter/log2_tab.o $BUILD_ROOT/libavutil/reverse.o $BUILD_ROOT/libavutil/log2_tab.o
$CC -lm -lz -shared --sysroot=$SYSROOT -Wl,--no-undefined -Wl,-z,noexecstack $E_LDFLAGS $BUILD_ROOT/libswresample/*.o $BUILD_ROOT/libswresample/x86/*.o \
$BUILD_ROOT/libswscale/*.o $BUILD_ROOT/libswscale/x86/*.o $BUILD_ROOT/libavfilter/*.o $BUILD_ROOT/libavformat/*.o \
$BUILD_ROOT/libavcodec/*.o $BUILD_ROOT/libavcodec/x86/*.o $BUILD_ROOT/compat/*.o $BUILD_ROOT/libavutil/*.o $BUILD_ROOT/libavutil/x86/*.o \
-o $PREFIX/lib/libffmpeg.so
###############################################################################
#
# MIPS build configuration
#
###############################################################################
PREFIX=$BASE/install/mips
BUILD_ROOT=$BASE/build/mips
PLATFORM=14
SYSROOT=$NDK/platforms/android-$PLATFORM/arch-mips/
TOOLCHAIN=$NDK/toolchains/mipsel-linux-android-4.9/prebuilt/linux-$HOST_ARCH
CROSS_PREFIX=$TOOLCHAIN/bin/mipsel-linux-android-
ARCH=mips
E_CFLAGS=
E_LDFLAGS=
EXTRA="--disable-asm"
build_one "$BUILD_ROOT" "$PREFIX" "$CROSS_PREFIX" "$ARCH" "$SYSROOT" \
"$E_CFLAGS" "$E_LDFLAGS" "$EXTRA"
