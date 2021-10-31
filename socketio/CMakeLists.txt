# TODO: determine CMAKE_SYSTEM_NAME on OS/390.  Currently assumes "OS/390".
cmake_minimum_required(VERSION 3.4)
project(libuv LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(GNUInstallDirs)
include(CTest)

cmake_dependent_option(LIBUV_BUILD_TESTS
  "Build the unit tests when BUILD_TESTING is enabled and we are the root project" ON
  "BUILD_TESTING;CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)

if(MSVC)
  list(APPEND uv_cflags /W4)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang|Clang|GNU")
  #list(APPEND uv_cflags -fvisibility=hidden --std=gnu89)
  list(APPEND uv_cflags -Wall -Wextra -Wstrict-prototypes)
  list(APPEND uv_cflags -Wno-unused-parameter)
endif()

set(uv_sources
    ./client/src/libuv/src/fs-poll.cpp
    ./client/src/libuv/src/idna.cpp
    ./client/src/libuv/src/inet.cpp
    ./client/src/libuv/src/random.cpp
    ./client/src/libuv/src/strscpy.cpp
    ./client/src/libuv/src/threadpool.cpp
    ./client/src/libuv/src/timer.cpp
    ./client/src/libuv/src/uv-common.cpp
    ./client/src/libuv/src/uv-data-getter-setters.cpp
    ./client/src/libuv/src/version.cpp)

set(uv_test_sources
    test/blackhole-server.cpp
    test/test-walk-handles.cpp
    test/test-watcher-cross-stop.cpp)

if(WIN32)
  list(APPEND media_defines SCY_WIN)
  list(APPEND uv_defines WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0600)
  list(APPEND uv_libraries
       advapi32
       iphlpapi
       psapi
       shell32
       user32
       userenv
       ws2_32)
  list(APPEND uv_sources
       ./client/src/libuv/src/win/async.cpp
       ./client/src/libuv/src/win/core.cpp
       ./client/src/libuv/src/win/detect-wakeup.cpp
       ./client/src/libuv/src/win/dl.cpp
       ./client/src/libuv/src/win/error.cpp
       ./client/src/libuv/src/win/fs.cpp
       ./client/src/libuv/src/win/fs-event.cpp
       ./client/src/libuv/src/win/getaddrinfo.cpp
       ./client/src/libuv/src/win/getnameinfo.cpp
       ./client/src/libuv/src/win/handle.cpp
       ./client/src/libuv/src/win/loop-watcher.cpp
       ./client/src/libuv/src/win/pipe.cpp
       ./client/src/libuv/src/win/thread.cpp
       ./client/src/libuv/src/win/poll.cpp
       ./client/src/libuv/src/win/process.cpp
       ./client/src/libuv/src/win/process-stdio.cpp
       ./client/src/libuv/src/win/signal.cpp
       ./client/src/libuv/src/win/snprintf.cpp
       ./client/src/libuv/src/win/stream.cpp
       ./client/src/libuv/src/win/tcp.cpp
       ./client/src/libuv/src/win/tty.cpp
       ./client/src/libuv/src/win/udp.cpp
       ./client/src/libuv/src/win/util.cpp
       ./client/src/libuv/src/win/winapi.cpp
       ./client/src/libuv/src/win/winsock.cpp)
  list(APPEND uv_test_libraries ws2_32)
  list(APPEND uv_test_sources ./client/src/libuv/src/win/snprintf.cpp test/runner-win.cpp)
else()
  list(APPEND uv_defines _FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE)
  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
    # Android has pthread as part of its c library, not as a separate
    # libpthread.so.
    list(APPEND uv_libraries pthread)
  endif()
  list(APPEND uv_sources
       ./client/src/libuv/src/unix/async.cpp
       ./client/src/libuv/src/unix/core.cpp
       ./client/src/libuv/src/unix/dl.cpp
       ./client/src/libuv/src/unix/fs.cpp
       ./client/src/libuv/src/unix/getaddrinfo.cpp
       ./client/src/libuv/src/unix/getnameinfo.cpp
       ./client/src/libuv/src/unix/loop-watcher.cpp
       ./client/src/libuv/src/unix/loop.cpp
       ./client/src/libuv/src/unix/pipe.cpp
       ./client/src/libuv/src/unix/poll.cpp
       ./client/src/libuv/src/unix/process.cpp
       ./client/src/libuv/src/unix/random-devurandom.cpp
       ./client/src/libuv/src/unix/signal.cpp
       ./client/src/libuv/src/unix/stream.cpp
       ./client/src/libuv/src/unix/tcp.cpp
       ./client/src/libuv/src/unix/thread.cpp
       ./client/src/libuv/src/unix/tty.cpp
       ./client/src/libuv/src/unix/udp.cpp)
  list(APPEND uv_test_sources test/runner-unix.cpp)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "AIX")
  list(APPEND uv_defines
       _ALL_SOURCE
       _LINUX_SOURCE_COMPAT
       _THREAD_SAFE
       _XOPEN_SOURCE=500)
  list(APPEND uv_libraries perfstat)
  list(APPEND uv_sources ./client/src/libuv/src/unix/aix.cpp)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
  list(APPEND uv_libs dl)
  list(APPEND uv_sources
       ./client/src/libuv/src/unix/android-ifaddrs.cpp
       ./client/src/libuv/src/unix/linux-core.cpp
       ./client/src/libuv/src/unix/linux-inotify.cpp
       ./client/src/libuv/src/unix/linux-syscalls.cpp
       #./client/src/libuv/src/unix/procfs-exepath.cpp
       ./client/src/libuv/src/unix/pthread-fixes.cpp
       #./client/src/libuv/src/unix/sysinfo-loadavg.cpp
)
endif()

if(APPLE OR CMAKE_SYSTEM_NAME MATCHES "Android|Linux|OS/390")
  list(APPEND uv_sources ./client/src/libuv/src/unix/proctitle.cpp)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "DragonFly|FreeBSD")
  list(APPEND uv_sources ./client/src/libuv/src/unix/freebsd.cpp)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "DragonFly|FreeBSD|NetBSD|OpenBSD")
  list(APPEND uv_sources ./client/src/libuv/src/unix/posix-hrtime.cpp ./client/src/libuv/src/unix/bsd-proctitle.cpp)
  list(APPEND uv_libraries kvm)
endif()

if(APPLE OR CMAKE_SYSTEM_NAME MATCHES "DragonFly|FreeBSD|NetBSD|OpenBSD")
  list(APPEND uv_sources ./client/src/libuv/src/unix/bsd-ifaddrs.cpp ./client/src/libuv/src/unix/kqueue.cpp)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
  list(APPEND uv_sources ./client/src/libuv/src/unix/random-getrandom.cpp)
endif()

if(APPLE OR CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
  list(APPEND uv_sources ./client/src/libuv/src/unix/random-getentropy.cpp)
endif()

if(APPLE)
  list(APPEND uv_defines _DARWIN_UNLIMITED_SELECT=1 _DARWIN_USE_64_BIT_INODE=1)
  list(APPEND uv_sources
       ./client/src/libuv/src/unix/darwin-proctitle.cpp
       ./client/src/libuv/src/unix/darwin.cpp
       ./client/src/libuv/src/unix/fsevents.cpp)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  list(APPEND uv_defines _GNU_SOURCE _POSIX_C_SOURCE=200112)
  list(APPEND uv_libraries dl rt)
  list(APPEND uv_sources
       ./client/src/libuv/src/unix/linux-core.cpp
       ./client/src/libuv/src/unix/linux-inotify.cpp
       ./client/src/libuv/src/unix/linux-syscalls.cpp
       #./client/src/libuv/src/unix/procfs-exepath.cpp
       ./client/src/libuv/src/unix/random-getrandom.cpp
       ./client/src/libuv/src/unix/random-sysctl.cpp
       #./client/src/libuv/src/unix/sysinfo-loadavg.cpp
)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
  list(APPEND uv_sources ./client/src/libuv/src/unix/netbsd.cpp)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
  list(APPEND uv_sources ./client/src/libuv/src/unix/openbsd.cpp)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "OS/390")
  list(APPEND uv_defines PATH_MAX=255)
  list(APPEND uv_defines _AE_BIMODAL)
  list(APPEND uv_defines _ALL_SOURCE)
  list(APPEND uv_defines _LARGE_TIME_API)
  list(APPEND uv_defines _OPEN_MSGQ_EXT)
  list(APPEND uv_defines _OPEN_SYS_FILE_EXT)
  list(APPEND uv_defines _OPEN_SYS_IF_EXT)
  list(APPEND uv_defines _OPEN_SYS_SOCK_EXT3)
  list(APPEND uv_defines _OPEN_SYS_SOCK_IPV6)
  list(APPEND uv_defines _UNIX03_SOURCE)
  list(APPEND uv_defines _UNIX03_THREADS)
  list(APPEND uv_defines _UNIX03_WITHDRAWN)
  list(APPEND uv_defines _XOPEN_SOURCE_EXTENDED)
  list(APPEND uv_sources
       ./client/src/libuv/src/unix/pthread-fixes.cpp
       ./client/src/libuv/src/unix/pthread-barrier.cpp
       ./client/src/libuv/src/unix/os390.cpp
       ./client/src/libuv/src/unix/os390-syscalls.cpp)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
  list(APPEND uv_defines __EXTENSIONS__ _XOPEN_SOURCE=500)
  list(APPEND uv_libraries kstat nsl sendfile socket)
  list(APPEND uv_sources ./client/src/libuv/src/unix/no-proctitle.cpp ./client/src/libuv/src/unix/sunos.cpp)
endif()

if(APPLE OR CMAKE_SYSTEM_NAME MATCHES "DragonFly|FreeBSD|Linux|NetBSD|OpenBSD")
  list(APPEND uv_test_libraries util)
endif()

add_library(uv SHARED ${uv_sources})
target_compile_definitions(uv
                           INTERFACE USING_UV_SHARED=1
                           PRIVATE ${uv_defines} BUILDING_UV_SHARED=1)
target_compile_options(uv PRIVATE ${uv_cflags})
target_include_directories(uv PUBLIC ./client/src/libuv/include PRIVATE ./client/src/libuv/src)
target_link_libraries(uv ${uv_libraries})

add_library(uv_a STATIC ${uv_sources})
target_compile_definitions(uv_a PRIVATE ${uv_defines})
target_compile_options(uv_a PRIVATE ${uv_cflags})
target_include_directories(uv_a PUBLIC ./client/src/libuv/include PRIVATE ./client/src/libuv/src)
target_link_libraries(uv_a ${uv_libraries})


file(GLOB SOURCES "./client/src/base/src/*.cpp" "./client/src/http_parser/*.cpp" "./client/src/json/src/*.cpp" )

list(APPEND SOURCES
./client/src/net/src/IP.cpp
#./client/src/net/src/SslConnection.cpp
./client/src/net/src/TcpServer.cpp
#./client/src/net/src/ssladapter.cpp
./client/src/net/src/PortManager.cpp
./client/src/net/src/TcpConnection.cpp 
./client/src/net/src/UdpSocket.cpp
./client/src/http/src/HTTPResponder.cpp 
#./client/src/http/src/HttpsConn.cpp  
./client/src/http/src/message.cpp    
./client/src/http/src/url.cpp
./client/src/http/src/HttpClient.cpp 
./client/src/http/src/client.cpp 
./client/src/http/src/packetizers.cpp
./client/src/http/src/websocket.cpp
./client/src/http/src/HttpConn.cpp 
./client/src/http/src/cookie.cpp 
./client/src/http/src/parser.cpp
./client/src/http/src/HttpServer.cpp  
./client/src/http/src/form.cpp    
./client/src/http/src/request.cpp
#./client/src/http/src/HttpsClient.cpp  
./client/src/http/src/httputil.cpp 
./client/src/http/src/response.cpp

./client/src/rtsp/ffmpeg/aac_ac3_parser.cpp
./client/src/rtsp/ffmpeg/ac3_parser.cpp
./client/src/rtsp/ffmpeg/ac3tab.cpp
./client/src/rtsp/ffmpeg/aes.cpp
./client/src/rtsp/ffmpeg/aes_ctr.cpp
./client/src/rtsp/ffmpeg/allcodecs.cpp
./client/src/rtsp/ffmpeg/allformats.cpp
./client/src/rtsp/ffmpeg/atomic.cpp
./client/src/rtsp/ffmpeg/audio_frame_queue.cpp
./client/src/rtsp/ffmpeg/audiointerleave.cpp
./client/src/rtsp/ffmpeg/avbuffer.cpp
./client/src/rtsp/ffmpeg/avc.cpp
./client/src/rtsp/ffmpeg/avfft.cpp
./client/src/rtsp/ffmpeg/avio.cpp
./client/src/rtsp/ffmpeg/aviobuf.cpp
./client/src/rtsp/ffmpeg/avpacket.cpp
./client/src/rtsp/ffmpeg/avparser.cpp
./client/src/rtsp/ffmpeg/avstring.cpp
./client/src/rtsp/ffmpeg/avtime.cpp
./client/src/rtsp/ffmpeg/bitstream.cpp
./client/src/rtsp/ffmpeg/bitstream_filters.cpp
./client/src/rtsp/ffmpeg/bprint.cpp
./client/src/rtsp/ffmpeg/bsf.cpp
./client/src/rtsp/ffmpeg/channel_layout.cpp
./client/src/rtsp/ffmpeg/codec_desc.cpp
./client/src/rtsp/ffmpeg/color_utils.cpp
./client/src/rtsp/ffmpeg/cpu.cpp
./client/src/rtsp/ffmpeg/crc.cpp
./client/src/rtsp/ffmpeg/cuvid.cpp
./client/src/rtsp/ffmpeg/dct.cpp
./client/src/rtsp/ffmpeg/dict.cpp
./client/src/rtsp/ffmpeg/error.cpp
./client/src/rtsp/ffmpeg/eval.cpp
./client/src/rtsp/ffmpeg/faandct.cpp
./client/src/rtsp/ffmpeg/fdctdsp.cpp
./client/src/rtsp/ffmpeg/fifo.cpp
./client/src/rtsp/ffmpeg/file_open.cpp
./client/src/rtsp/ffmpeg/format.cpp
./client/src/rtsp/ffmpeg/frame_thread_encoder.cpp
./client/src/rtsp/ffmpeg/h264_mp4toannexb_bsf.cpp
./client/src/rtsp/ffmpeg/hwcontext.cpp
./client/src/rtsp/ffmpeg/id3v1.cpp
./client/src/rtsp/ffmpeg/id3v2.cpp
./client/src/rtsp/ffmpeg/imgutils.cpp
./client/src/rtsp/ffmpeg/intmath.cpp
./client/src/rtsp/ffmpeg/isom.cpp
./client/src/rtsp/ffmpeg/libfdk-aacdec.cpp
./client/src/rtsp/ffmpeg/libfdk-aacenc.cpp
./client/src/rtsp/ffmpeg/live.cpp
./client/src/rtsp/ffmpeg/livethread.cpp
./client/src/rtsp/ffmpeg/log.cpp
./client/src/rtsp/ffmpeg/mathematics.cpp
./client/src/rtsp/ffmpeg/mem.cpp
./client/src/rtsp/ffmpeg/metadata.cpp
./client/src/rtsp/ffmpeg/mov_chan.cpp
./client/src/rtsp/ffmpeg/movenc.cpp
./client/src/rtsp/ffmpeg/movenccenc.cpp
./client/src/rtsp/ffmpeg/movenchint.cpp
./client/src/rtsp/ffmpeg/mpeg12data.cpp
./client/src/rtsp/ffmpeg/mpeg4_unpack_bframes_bsf.cpp
./client/src/rtsp/ffmpeg/mpeg4audio.cpp
./client/src/rtsp/ffmpeg/mpegaudiodata.cpp
./client/src/rtsp/ffmpeg/mpegpicture.cpp
./client/src/rtsp/ffmpeg/mpegutils.cpp
./client/src/rtsp/ffmpeg/mpegvideodsp.cpp
./client/src/rtsp/ffmpeg/mpegvideoencdsp.cpp
./client/src/rtsp/ffmpeg/mux.cpp
./client/src/rtsp/ffmpeg/muxframe.cpp
./client/src/rtsp/ffmpeg/network.cpp
./client/src/rtsp/ffmpeg/opt.cpp
./client/src/rtsp/ffmpeg/options.cpp
./client/src/rtsp/ffmpeg/options_avformat.cpp
./client/src/rtsp/ffmpeg/os_support.cpp
./client/src/rtsp/ffmpeg/parseutils.cpp
./client/src/rtsp/ffmpeg/pixdesc.cpp
./client/src/rtsp/ffmpeg/profiles.cpp
./client/src/rtsp/ffmpeg/protocols.cpp
./client/src/rtsp/ffmpeg/pthread.cpp
./client/src/rtsp/ffmpeg/pthread_frame.cpp
./client/src/rtsp/ffmpeg/pthread_slice.cpp
./client/src/rtsp/ffmpeg/random_seed.cpp
./client/src/rtsp/ffmpeg/ratecontrol.cpp
./client/src/rtsp/ffmpeg/rational.cpp
./client/src/rtsp/ffmpeg/raw.cpp
./client/src/rtsp/ffmpeg/rawutils.cpp
./client/src/rtsp/ffmpeg/rdft.cpp
./client/src/rtsp/ffmpeg/reverse.cpp
./client/src/rtsp/ffmpeg/riff.cpp
./client/src/rtsp/ffmpeg/riffenc.cpp
./client/src/rtsp/ffmpeg/rl.cpp
./client/src/rtsp/ffmpeg/samplefmt.cpp
./client/src/rtsp/ffmpeg/sha.cpp
./client/src/rtsp/ffmpeg/stereo3d.cpp
./client/src/rtsp/ffmpeg/timecode.cpp
./client/src/rtsp/ffmpeg/utils.cpp
./client/src/rtsp/ffmpeg/utils_avformat.cpp
./client/src/rtsp/ffmpeg/utils_avutil.cpp
./client/src/rtsp/ffmpeg/vpcc.cpp


)

add_executable(main  mainhttp.cpp  ${SOURCES})
target_compile_definitions(main PRIVATE ${media_defines})
target_include_directories(main PUBLIC ./client/src/base/include ./client/src/http_parser ./client/src/json/include ./client/src/net/include ./client/src/http/include ./client/src/libuv/include ./client/src/libuv/src  ./client/src/signal/include ./client/src/rtsp/ffmpeg )
target_link_libraries(main  PUBLIC uv_a )


add_library(httpStream STATIC ${SOURCES})
target_compile_definitions(httpStream PRIVATE ${media_defines})
target_include_directories(httpStream PUBLIC ./client/src/base/include ./client/src/http_parser ./client/src/json/include ./client/src/net/include ./client/src/http/include ./client/src/libuv/include ./client/src/libuv/src  ./client/src/signal/include ./client/src/rtsp/ffmpeg )
target_link_libraries(httpStream  PUBLIC uv_a )



if(UNIX)
  # Now for some gibbering horrors from beyond the stars...
  foreach(x ${uv_libraries})
    set(LIBS "${LIBS} -l${x}")
  endforeach(x)
  file(STRINGS configure.ac configure_ac REGEX ^AC_INIT)
  string(REGEX MATCH [0-9]+[.][0-9]+[.][0-9]+ PACKAGE_VERSION "${configure_ac}")
  string(REGEX MATCH ^[0-9]+ UV_VERSION_MAJOR "${PACKAGE_VERSION}")
  # The version in the filename is mirroring the behaviour of autotools.
  set_target_properties(uv PROPERTIES VERSION ${UV_VERSION_MAJOR}.0.0
              SOVERSION ${UV_VERSION_MAJOR})
  set(includedir ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
  set(libdir ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
  set(prefix ${CMAKE_INSTALL_PREFIX})
  configure_file(libuv.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libuv.pc @ONLY)

  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
  install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR})
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libuv.pc
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
  install(TARGETS uv LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(TARGETS uv_a ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

if(WIN32)
  install(DIRECTORY include/ DESTINATION include)
  install(FILES LICENSE DESTINATION .)
  install(TARGETS uv uv_a
          RUNTIME DESTINATION lib/$<CONFIG>
          ARCHIVE DESTINATION lib/$<CONFIG>)
endif()
