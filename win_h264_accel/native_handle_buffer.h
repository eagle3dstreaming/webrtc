

#ifndef _NATIVE_FRAME_H_
#define _NATIVE_FRAME_H_

#include "api/video/video_frame_buffer.h"
#include "media/base/video_common.h"

namespace webrtc {

class NativeHandleBuffer : public VideoFrameBuffer {
 public:
  NativeHandleBuffer(void* native_handle, int width, int height)
    : native_handle_(native_handle),
    width_(width),
    height_(height) { }

  Type type() const override {
    return Type::kNative;
  }

  int width() const override {
    return width_;
  }
  int height() const override {
    return height_;
  }

  void* native_handle() const {
    return native_handle_;
  }

  virtual cricket::FourCC fourCC() const = 0;

 protected:
  void* native_handle_;
  const int width_;
  const int height_;
};
}  // namespace webrtc

#endif  // _NATIVE_FRAME_H_
