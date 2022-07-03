# socketIO
C++ socketIO Signaller for all the plateform

It works with Android, IOS and windows and linux

I am using it at media streaming, please check mediaserver respository.

# socketIO
C++ socketIO Signaller for all the plateform

It works with Android, IOS and windows and linux

I am using it at media streaming, please check mediaserver respository.


mkdir build 

cd build 

on windows

## Widnows
mkdir build 

cd build

cmake -G "Visual Studio 16 2019" -A x64 -T ClangCL ..






gn gen out/m89  --args="target_cpu=\"x64\" is_debug=true symbol_level=2 is_component_build=false is_clang=true use_lld=false treat_warnings_as_errors=false use_rtti=true rtc_include_tests =false rtc_build_examples=true  use_lld=false  is_clang=true is_component_build=false use_lld=false"

ninja -C out/m89/ webrtc


ninja -C out/m89/ mp4

