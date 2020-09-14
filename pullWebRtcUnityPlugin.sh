#!/bin/bash

set -eu # stop on error
set -x  # print commands

# install/update depot-tools
if [ ! -d "depot_tools" ]; then
  git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
else
  cd depot_tools
  git pull
  cd ..
fi

BASE_PATH=`pwd`
export PATH=$PATH:${BASE_PATH}/depot_tools

mkdir webrtc_android/

cd webrtc_android

fetch --nohooks webrtc_android. 

cd src git checkout branch-heads/m75 Then type gclient sync

gclient sync

git remote add arvind https://github.com/scopear/webrtc.git

git remote update

git checkout Multiplex_UnityPlugin

# then run generateUnityPlugin.sh
#  for windows we do not have batch file it is manual
# if you haven’t installed a copy of git, open https://git-for-windows.github.io/, download and install, suggest to select “Use Git from Windows Command Prompt” during installation.

# Fetch depot_tools:
# follow http://dev.chromium.org/developers/how-tos/install-depot-tools, use 7z to extract to E:\webrtc\depot_tools

# Run depot_tools:
# http://www.chromium.org/developers/how-tos/build-instructions-windows

# mkdir webrtc-checkout cd webrtc-checkout fetch --nohooks webrtc

# do not forget to set for using visual studio for builing code
#  set DEPOT_TOOLS_WIN_TOOLCHAIN=0

#  cd src git branch -r 
# git checkout branch-heads/m76

#git remote add arvind https://github.com/scopear/webrtc.git

#git remote update

#git checkout Multiplex_UnityPlugin
