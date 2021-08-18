**WebRTC is a free, open software project** that provides browsers and mobile
applications with Real-Time Communications (RTC) capabilities via simple APIs.
The WebRTC components have been optimized to best serve this purpose.

### Development

See http://www.webrtc.org/native-code/development for instructions on how to get
started developing with the native code.






## Purpose of this Document
a) This document helps in compiling Eagle3D PixelStreaming Plugin
b) Socketio plugin
c) Unity C++ plugin
d) Broadcasting HLS, Mpeg Dash FMp4
e) HTTP Streamer


## _Pixel Streamig Compilation_

git clone git@github.com:eagle3dstreaming/UnrealEngine.git

git checkout seagull-dev4.26

git lfs fetch --all

git lfs checkout 


run Setup.bat  to install dependent modules

run GenerateProjectFiles to genearte poject files

install missing Visual Studio 2019 modules 


compile with sln solution file

package game

 git checkout branch-heads/m76


## _Webrtc Compilation_

Please make sure UnrealEngine is at path

c:/UnrealEngine/

install 

To obtain the latest version of DbgHelp.dll, go to https://developer.microsoft.com/windows/downloads/windows-10-sdk and

install it

Fetch depot_tools: follow https://sourcey.com/articles/building-and-installing-webrtc-on-windows use 7z to extract to c:\webrtc\depot_tools


set system path

open git cmd not git basah

mkdir webrtc-checkout  cd webrtc-checkout fetch --nohooks webrtc

do not forget to set for using visual studio for builing code


cd src

set DEPOT_TOOLS_WIN_TOOLCHAIN=0

cd src git branch -r git checkout branch-heads/m75

git remote add arvind git@github.com:akumrao/webrtcwithsocketio.git

git remote update

git checkout sfu

gclient sync


pwd

webrtc-checkout/src

cp Ue4/buildPatch.diff to build

cd build 

git apply buildPatch.diff


cd ..

cp Ue4/third_partyPatch.diff to third_party

cd third_party

git apply third_partyPatch.diff

cd ..

cp Ue4/srtpPatch.diff to third_party/libsrtp

cd third_party/libsrtp

git apply srtpPatch.diff

cd ..

pwd

webrtc-checkout/src


gn gen out/x64/Release  --args="target_cpu=\"x64\" use_rtti=true is_debug=false symbol_level=0 enable_iterator_debugging=false rtc_build_tools=false rtc_build_examples=false rtc_include_tests=false enable_precompiled_headers=false use_cxx11=false use_custom_libcxx=false use_custom_libcxx_for_host=false rtc_build_ssl=false rtc_ssl_root=\"c:\UnrealEngine\Engine\Source\ThirdParty\OpenSSL\1.1.1\Include\Win64\VS2015\" is_clang=false is_component_build=false use_lld=false"


ninja -C out/x64/Release webrtc

gn gen out/x64/Debug  --args="target_cpu=\"x64\" use_rtti=true is_debug=true symbol_level=2 rtc_build_tools=false rtc_build_examples=true rtc_include_tests=false enable_precompiled_headers=false use_cxx11=false use_custom_libcxx=false use_custom_libcxx_for_host=false rtc_build_ssl=false rtc_ssl_root=\"E:\UnrealEngine\Engine\Source\ThirdParty\OpenSSL\1.1.1\Include\Win64\VS2015\" is_clang=false is_component_build=false use_lld=false"

ninja -C out/x64/Debug webrtc



## Features
- Lite -ICE
- WebRTC Statistics 
- M76 webrtc version
- Audio Mute Control
- Onfly Webrtc parameter Settings
- Better Frame Adaption 



## PATH and Commands
For running game project for Pixel streamming
./project -AudioMixer -PixelStreamingIP=localhost -PixelStreamingPort=8888 -RenderOffScreen -log

//Upload project to control panel 
https://connector.eagle3dstreaming.com:500/nelsonww/ControlPanel/083057




to copy header files of webrtc
find . -iname "*.h" | xargs -If cp --parents f /c/arvind







## HTTP Streamer

set DEPOT_TOOLS_WIN_TOOLCHAIN=0

gn gen out/debugx64 --args="is_debug=true is_clang=true symbol_level=2 use_rtti=true target_cpu=\"x64\""

ninja -C out/debugx64 signaler
ninja -C out/debugx64 socketio
ninja -C out/debugx64 testhtml




## To comple with Visual studio 


mkdir build 

cd build 

on windows

cmake.exe ..  -G  "Visual Studio 16 2019"