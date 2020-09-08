//
// Created by root on 8/9/20.
//

#include "examples/unityplugin/augVcmCapturer.h"


void AugVideoCapturer::OnFrame(const webrtc::VideoFrame& original_frame) {
    int cropped_width = 0;
    int cropped_height = 0;
    int out_width = 0;
    int out_height = 0;

    broadcaster_.OnFrame(original_frame);
    return;
}

void AugVcmCapturer::OnFrame(const webrtc::VideoFrame& frame)
{
    AugVideoCapturer::OnFrame(frame);
}


 rtc::scoped_refptr<CapturerTrackSource>CapturerTrackSource::Create() {

    std::unique_ptr< AugVcmCapturer> capturer = absl::WrapUnique(
            AugVcmCapturer::Create());
    if (!capturer) {
        return nullptr;
    }
    return new rtc::RefCountedObject<CapturerTrackSource>(std::move(capturer));
}

 void CapturerTrackSource::OnFrame(const webrtc::VideoFrame& frame)
{
    capturer_->OnFrame( frame  );
}


