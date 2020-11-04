
#ifndef EXAMPLES_UNITYPLUGIN_VIDEO_OBSERVER_H_
#define EXAMPLES_UNITYPLUGIN_VIDEO_OBSERVER_H_

//#define DEBUG_ONFRAME
#include <mutex>

#include "api/media_stream_interface.h"
#include "api/video/video_sink_interface.h"
#include "examples/unityplugin/unity_plugin_apis.h"

class VideoObserver : public rtc::VideoSinkInterface<webrtc::VideoFrame> {
 public:
  VideoObserver(int clinetId):nClientID(clinetId) {
#if DEBUG_ONFRAME
    nCount =0;
#endif
  }
  ~VideoObserver() {}
  void SetVideoCallback(I420FRAMEREADY_CALLBACK callback);

 protected:
  // VideoSinkInterface implementation
  void OnFrame(const webrtc::VideoFrame& frame) override;

 private:
  I420FRAMEREADY_CALLBACK OnI420FrameReady = nullptr;
  std::mutex mutex;

    int nClientID;
#if DEBUG_ONFRAME
    int nCount;
#endif
};

#endif  // EXAMPLES_UNITYPLUGIN_VIDEO_OBSERVER_H_
