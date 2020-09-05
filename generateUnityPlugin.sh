#!/bin/bash

set -eu # stop on error
set -x  # print commands


#BASE_PATH=`pwd`
#export PATH=$PATH:${BASE_PATH}/depot_tools




rm -f build/android/android_only_jni_exports.lst
rm -f build/android/android_only_explicit_jni_exports.lst
rm -f ../libjingle_peerconnection_so.so
rm -f ../libwebrtc_unity.jar

export PATH=/export/webrtc/depot_tools:$PATH

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

cd ..

