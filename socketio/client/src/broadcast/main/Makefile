################################################################################
### CUSTOMIZE BELOW HERE #######################################################

LIVE555_ROOT = /workspace/live
#WEBRTC_PATH = /data/delete/mediaserverdocker/webrtc_android/src/
WEBRTC_PATH = /workspace/webrtc_android/src/

CC	= gcc
CXX     = g++
TARGET=runStreamer # define the name of the executable
SOURCES=$(wildcard *.cpp ../src/*.cpp ../../libuv/src/*.cpp ../../libuv/src/unix/*.cpp ../../base/src/*.cpp ../../crypto1/src/*.cpp ../../net/src/*.cpp ../../http/src/*.cpp ../../http_parser/*.cpp ../../signal/src/*.cpp ../../ffmpeg/src/*.cpp  ../../ffmpeg/ffmpeg_cpp/*.cpp ../../ffmpeg/ffmpeg_cpp/codec/*.cpp  ../../ffmpeg/ffmpeg_cpp/codec/x86/*.cpp ../../ffmpeg/ffmpeg_cpp/audio/*.cpp ../../ffmpeg/ffmpeg_cpp/mp4Probe/*.cpp  )

CXXFLAGS   := -g -std=c++14 -Wno-inconsistent-missing-override -Wno-uninitialized -Wno-attributes -fPIC -fno-permissive -Wno-literal-suffix

CPPFLAGS   := -I../../json/include/ -I../include -I../../signal/include -I../../crypto/include/ -I../../http/include/ -I../../base/include/ -I../../net/include/ -I../../libuv/include -I../../libuv/src -I../../../build/ -I../../http_parser/ -I../webrtc/include -I../../ffmpeg/include -DOPENSSL_IS_BORINGSSL -DWEBRTC_POSIX  -DWEBRTC_INCLUDE_DIRS=/workspace/webrtc_android/src -I../../ffmpeg/ffmpeg_cpp/ -I../../ffmpeg/ffmpeg_cpp/compat/atomics/gcc/ -I../../ffmpeg/ffmpeg_cpp/codec -I../../ffmpeg/ffmpeg_cpp/codec/x86/ -I../../ffmpeg/ffmpeg_cpp/audio/  -I../../ffmpeg/ffmpeg_cpp/mp4Probe/ \
               -I${WEBRTC_PATH} -I${WEBRTC_PATH}/third_party/abseil-cpp
#LDFLAGS   := -L/export/views/video/ffmpeg/Obj/
#/export/views/video/ffmpeg/libavutil/libavutil.a /export/views/video/ffmpeg/libavcodec/libavcodec.a
#LDLIBS    := -lpthread -lm -ldl -lavformat -lavfilter -lavcodec -lavutil -lswscale -lavdevice -lswresample

LDLIBS    := -lpthread -lm -ldl -lz -lfdk-aac  -no-pie

SLIBS     := ${WEBRTC_PATH}/out/m75/obj/libwebrtc.a   /usr/local/lib/libx264.a



################################################################################
### DO NOT EDIT THE FOLLOWING LINES ############################################


# define list of objects
OBJSC=$(SOURCES:.cpp=.o)
OBJS=$(OBJSC:.cpp=.o)

# the target is obtained linking all .o files
all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) $(SLIBS) $(LDFLAGS) $(LDLIBS) -o $(TARGET)

purge: clean
	rm -f $(TARGET)

clean:
	rm -f  $(OBJS)

################################################################################
################################################################################
