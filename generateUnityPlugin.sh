#!/bin/bash

set -eu # stop on error
set -x  # print commands


#BASE_PATH=`pwd`
#export PATH=$PATH:${BASE_PATH}/depot_tools




rm -f build/android/android_only_jni_exports.lst
rm -f build/android/android_only_explicit_jni_exports.lst
rm -f ../libjingle_peerconnection_so.so
rm -f ../libwebrtc_unity.jar

export PATH=/webrtc/depot_tools:$PATH

#
# export targets

echo '{ global: *; };' >  build/android/android_only_jni_exports.lst
echo '{ global: *; };' >  build/android/android_only_explicit_jni_exports.lst

# generate ninja build files
gn gen out/Android --args='target_os="android" target_cpu="arm"'

# build library .so file
ninja -C out/Android webrtc_unity_plugin

# .... wait ....

# copy and rename .so file to ~/webrtc
cp out/Android/libwebrtc_unity_plugin.so ../libjingle_peerconnection_so.so

# build java library
ninja -C out/Android libwebrtc_unity

# .. wait ..

# copy jar file to ~/webrtc
cp out/Android/lib.java/examples/libwebrtc_unity.jar ../.

#cp ../libjingle_peerconnection_so.so  /experiment/AugmentedReality/webrtc-ar-demo-socketio/Assets/Plugins/Android/libs/armeabi-v7a

cd ..

# For widnows webrtc dll are generated manually
# 
# set DEPOT_TOOLS_WIN_TOOLCHAIN=0


#gn gen out/debugx64 --args="is_debug=true rtc_use_h264=true proprietary_codecs=true ffmpeg_branding=\"Chrome\" use_rtti=true target_cpu=\"x64\""

#ninja -C out/debugx64 webrtc_unity_plugin

