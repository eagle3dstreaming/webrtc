/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <modules/video_coding/codecs/multiplex/include/augmented_video_frame_buffer.h>
#include "examples/unityplugin/video_observer.h"

#if DEBUG_ONFRAME
#include "rtc_base/logging.h"
#endif

void VideoObserver::SetVideoCallback(I420FRAMEREADY_CALLBACK callback) {
  std::lock_guard<std::mutex> lock(mutex);

#if DEBUG_ONFRAME
  RTC_LOG(INFO) <<"SetVideoCallback " <<  nClientID;
  RTC_LOG(INFO) <<"SetVideoCallback " <<  (int)callback;
#endif

  OnI420FrameReady = callback;
}

void VideoObserver::OnFrame(const webrtc::VideoFrame& frame) {


  std::unique_lock<std::mutex> lock(mutex);
  if (!OnI420FrameReady)
    return;

#if DEBUG_ONFRAME
  if(nCount %25 == 0 && nClientID > 1 )
    RTC_LOG(INFO) <<"clientID " <<  nClientID;
#endif


    rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer(
      frame.video_frame_buffer());
  if(buffer->type() == webrtc::VideoFrameBuffer::Type::kAugmented ) {

    uint8_t* augData = ((webrtc::AugmentedVideoFrameBuffer*)buffer.get())->GetAugmentingData();
    uint16_t augSize =  ((webrtc::AugmentedVideoFrameBuffer*)buffer.get())->GetAugmentingDataSize() ;

    rtc::scoped_refptr<webrtc::I420BufferInterface> i420_buffer =
            buffer->ToI420();
    OnI420FrameReady(nClientID, i420_buffer->DataY(), i420_buffer->DataU(),
                     i420_buffer->DataV(), augData, i420_buffer->StrideY(),
                     i420_buffer->StrideU(), i420_buffer->StrideV(), augSize,
                     frame.width(), frame.height());

  }
  else if (buffer->type() != webrtc::VideoFrameBuffer::Type::kI420A) {
    rtc::scoped_refptr<webrtc::I420BufferInterface> i420_buffer =
        buffer->ToI420();
    OnI420FrameReady(nClientID,i420_buffer->DataY(), i420_buffer->DataU(),
                     i420_buffer->DataV(), nullptr, i420_buffer->StrideY(),
                     i420_buffer->StrideU(), i420_buffer->StrideV(), 0,
                     frame.width(), frame.height());

  } else {
    // The buffer has alpha channel.
    const webrtc::I420ABufferInterface* i420a_buffer = buffer->GetI420A();

    OnI420FrameReady(nClientID,i420a_buffer->DataY(), i420a_buffer->DataU(),
                     i420a_buffer->DataV(), i420a_buffer->DataA(),
                     i420a_buffer->StrideY(), i420a_buffer->StrideU(),
                     i420a_buffer->StrideV(), i420a_buffer->StrideA(),
                     frame.width(), frame.height());
  }
}
