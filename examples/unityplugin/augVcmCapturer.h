//
// Created by Arvind Umrao on 8/9/20.
//

#ifndef ANDROID_AR_DEMO_SOCKETIO_AUGVCMCAPTURER_H
#define ANDROID_AR_DEMO_SOCKETIO_AUGVCMCAPTURER_H

#include <utility>
#include <thread>
#include <atomic>
#include <chrono>

#include "pc/video_track_source.h"
#include "modules/video_capture/video_capture_factory.h"
#include <modules/video_coding/codecs/multiplex/include/augmented_video_frame_buffer.h>
#include <media/base/video_broadcaster.h>
#include <media/base/video_adapter.h>


#include "api/scoped_refptr.h"
#include "api/video/i420_buffer.h"
#include "api/video/video_frame_buffer.h"
#include "api/video/video_rotation.h"
#include <algorithm>
#include <third_party/libyuv/include/libyuv.h>


class AugVideoCapturer : public rtc::VideoSourceInterface<webrtc::VideoFrame> {
public:
//    class FramePreprocessor {
//    public:
//        virtual ~FramePreprocessor() = default;
//
//        virtual webrtc::VideoFrame Preprocess(const webrtc::VideoFrame& frame) = 0;
//    };
//

    ~AugVideoCapturer() = default;

    void OnFrame(const webrtc::VideoFrame& original_frame) ;


    rtc::VideoSinkWants GetSinkWants() {
        return broadcaster_.wants();
    }

    void AddOrUpdateSink(
            rtc::VideoSinkInterface<webrtc::VideoFrame>* sink,
            const rtc::VideoSinkWants& wants) {
        broadcaster_.AddOrUpdateSink(sink, wants);
        UpdateVideoAdapter();
    }

    void RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink) {
        broadcaster_.RemoveSink(sink);
        UpdateVideoAdapter();
    }

    void UpdateVideoAdapter() {
        video_adapter_.OnSinkWants(broadcaster_.wants());
    }

//    webrtc::VideoFrame MaybePreprocess(const webrtc::VideoFrame& frame) {
//        rtc::CritScope crit(&lock_);
//        if (preprocessor_ != nullptr) {
//            return preprocessor_->Preprocess(frame);
//        } else {
//            return frame;
//        }
//    }

    rtc::CriticalSection lock_;
    //std::unique_ptr<FramePreprocessor> preprocessor_ RTC_GUARDED_BY(lock_);
    rtc::VideoBroadcaster broadcaster_;
    cricket::VideoAdapter video_adapter_;
};

class AugVcmCapturer : public AugVideoCapturer,
                        public rtc::VideoSinkInterface<webrtc::VideoFrame> {
public:

    AugVcmCapturer() {} /*: renderThread(nullptr) */

    bool Init() {
        //std::unique_ptr<VideoCaptureModule::DeviceInfo> device_info(
        //      VideoCaptureFactory::CreateDeviceInfo());
        // renderThread = new std::thread(&AugVcmCapturer::updateFunc, this);
        return true;
    }

    void OnFrame(const webrtc::VideoFrame& frame);


    static AugVcmCapturer* Create() {

        std::unique_ptr<AugVcmCapturer> vcm_capturer(new AugVcmCapturer());

        if (!vcm_capturer->Init()) {
            RTC_LOG(LS_WARNING) << "Failed to create VcmCapturer()";
            return nullptr;
        }
        return vcm_capturer.release();
    }

    virtual ~AugVcmCapturer() {
    }

    webrtc::VideoCaptureCapability capability_;
};

class CapturerTrackSource : public webrtc::VideoTrackSource, public rtc::VideoSinkInterface<webrtc::VideoFrame>  {
public:
    static rtc::scoped_refptr<CapturerTrackSource> Create();

    virtual void OnFrame(const webrtc::VideoFrame& frame) override;

protected:
    explicit CapturerTrackSource(
            std::unique_ptr<AugVcmCapturer> capturer)
            : VideoTrackSource(/*remote=*/false), capturer_(std::move(capturer)) {}

private:
    rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
        return capturer_.get();
    }
    std::unique_ptr<AugVcmCapturer> capturer_;
};


#endif //ANDROID_AR_DEMO_SOCKETIO_AUGVCMCAPTURER_H
