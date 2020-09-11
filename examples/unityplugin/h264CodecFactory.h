//
// Created by root on 11/9/20.
//

#ifndef ANDROID_AR_DEMO_SOCKETIO_H264CODECFACTORY_H
#define ANDROID_AR_DEMO_SOCKETIO_H264CODECFACTORY_H

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "api/video_codecs/sdp_video_format.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "rtc_base/checks.h"

namespace webrtc {
    namespace h264 {

// An encoder factory producing encoders by calling a supplied create
// function.
        class H264VideoEncoderFactory final : public VideoEncoderFactory {
        public:
            explicit H264VideoEncoderFactory(
                    std::function<std::unique_ptr<VideoEncoder>()> create)
                    : create_([create](const SdpVideoFormat&) { return create(); }) {}
            explicit H264VideoEncoderFactory(
                    std::function<std::unique_ptr<VideoEncoder>(const SdpVideoFormat&)>
                    create)
                    : create_(std::move(create)) {}

            // Unused by tests.
            std::vector<SdpVideoFormat> GetSupportedFormats() const override {
               // RTC_NOTREACHED();
                //return {};
                const webrtc::SdpVideoFormat h264_format("H264");
                const std::vector<webrtc::SdpVideoFormat> supported_formats = {h264_format};
                return supported_formats;
            }

            CodecInfo QueryVideoEncoder(
                    const SdpVideoFormat& /* format */) const override {
                CodecInfo codec_info;
                codec_info.is_hardware_accelerated = false;
                codec_info.has_internal_source = false;
                return codec_info;
            }

            std::unique_ptr<VideoEncoder> CreateVideoEncoder(
                    const SdpVideoFormat& format) override {
                return create_(format);
            }

        private:
            const std::function<std::unique_ptr<VideoEncoder>(const SdpVideoFormat&)>
                    create_;
        };


        // A decoder factory producing decoders by calling a supplied create function.
        class H264VideoDecoderFactory final : public VideoDecoderFactory {
        public:
            explicit H264VideoDecoderFactory(
                    std::function<std::unique_ptr<VideoDecoder>()> create)
                    : create_([create](const SdpVideoFormat&) { return create(); }) {}
            explicit H264VideoDecoderFactory(
                    std::function<std::unique_ptr<VideoDecoder>(const SdpVideoFormat&)>
                    create)
                    : create_(std::move(create)) {}

            // Unused by tests.
            std::vector<SdpVideoFormat> GetSupportedFormats() const override {
               // RTC_NOTREACHED();
               // return {};
                const webrtc::SdpVideoFormat h264_format("H264");
                const std::vector<webrtc::SdpVideoFormat> supported_formats = {h264_format};
                return supported_formats;
            }

            std::unique_ptr<VideoDecoder> CreateVideoDecoder(
                    const SdpVideoFormat& format) override {
                return create_(format);
            }

        private:
            const std::function<std::unique_ptr<VideoDecoder>(const SdpVideoFormat&)>
                    create_;
        };

    }  // namespace test
}  // namespace webrtc

#endif //ANDROID_AR_DEMO_SOCKETIO_H264CODECFACTORY_H
