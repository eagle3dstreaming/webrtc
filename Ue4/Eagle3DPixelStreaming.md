# Eagle3DPixelStreaming Doc

Authored by Arvind Umrao.

email id akumrao@yahoo.com

Version 0.1

## Purpose of this Document
This document helps in compiling Eagle3D PixelStreaming Plugin


## Features this repoistory provides
- Lite -ICE
- WebRTC Statistics 
- M76 webrtc version, rev.28114
- Audio Mute Control
- Onfly Webrtc parameter Settings
- Better Frame Adaption 



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

cd src git branch -r git checkout branch-heads/m76

git remote add arvind git@github.com:eagle3dstreaming/webrtc.git

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

cp Ue4srtppatch.diff to third_party\libsrtp

cd third_party/libsrtp

git apply srtppatch.diff

cd ..

pwd

webrtc-checkout/src


gn gen out/x64/Release  --args="target_cpu=\"x64\" use_rtti=true is_debug=false symbol_level=0 enable_iterator_debugging=false rtc_build_tools=false rtc_build_examples=false rtc_include_tests=false enable_precompiled_headers=false use_cxx11=false use_custom_libcxx=false use_custom_libcxx_for_host=false rtc_build_ssl=false rtc_ssl_root=\"c:\UnrealEngine\Engine\Source\ThirdParty\OpenSSL\1.1.1\Include\Win64\VS2015\" is_clang=false is_component_build=false use_lld=false"


ninja -C out/x64/Release webrtc


to copy header files of webrtc
find . -iname "*.h" | xargs -If cp --parents f /c/arvind

to copy lib
out\x64\Release\obj\webrtc.lib to C:\UnrealEngine\Engine\Source\ThirdParty\WebRTC\rev.28114\Lib\Win64\VS2015\Release



## Command line to run game with pixel streamimg eabled
For running game project for Pixel streamming
./project -AudioMixer -PixelStreamingIP=localhost -PixelStreamingPort=8888 -RenderOffScreen -log

##Upload project to control panel
https://connector.eagle3dstreaming.com:500/nelsonww/ControlPanel/083057









