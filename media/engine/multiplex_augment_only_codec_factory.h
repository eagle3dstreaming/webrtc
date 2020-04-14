/*
 *  Copied and modified multiplex_codec_factory.h
 */

#ifndef MEDIA_ENGINE_MULTIPLEX_AUGMENT_ONLY_CODEC_FACTORY_H_
#define MEDIA_ENGINE_MULTIPLEX_AUGMENT_ONLY_CODEC_FACTORY_H_

#include <memory>
#include <vector>

#include "api/video_codecs/sdp_video_format.h"
#include "api/video_codecs/video_decoder.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_encoder.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "rtc_base/system/rtc_export.h"

namespace webrtc {
// Multiplex codec is a completely modular/optional codec that allows users to
// send more than a frame's opaque content(RGB/YUV) over video channels.
// - Allows sending augmenting data over the wire attached to the frame. This
// attached data portion is not encoded in any way and sent as it is. Users can
// input AugmentedVideoFrameBuffer and can expect the same interface as the
// decoded video frame buffer.
// - Showcases an example of how to add a custom codec in webrtc video channel.
// How to use it end-to-end:
// - Wrap your existing VideoEncoderFactory implemention with
// MultiplexAugmentOnlyEncoderFactory and VideoDecoderFactory implemention with
// MultiplexAugmentOnlyDecoderFactory below. For actual coding, multiplex creates encoder
// and decoder instance(s) using these factories.
// - Use Multiplex*coderFactory classes in CreatePeerConnectionFactory() calls.
// - Select "multiplex" codec in SDP negotiation.
class RTC_EXPORT MultiplexAugmentOnlyEncoderFactory : public VideoEncoderFactory {
 public:
  // |supports_augmenting_data| defines if the encoder would support augmenting
  // data. If set, the encoder expects to receive video frame buffers of type
  // AugmentedVideoFrameBuffer.
  MultiplexAugmentOnlyEncoderFactory(std::unique_ptr<VideoEncoderFactory> factory,
                          bool supports_augmenting_data = false);

  std::vector<SdpVideoFormat> GetSupportedFormats() const override;
  CodecInfo QueryVideoEncoder(const SdpVideoFormat& format) const override;
  std::unique_ptr<VideoEncoder> CreateVideoEncoder(
      const SdpVideoFormat& format) override;

 private:
  std::unique_ptr<VideoEncoderFactory> factory_;
  const bool supports_augmenting_data_;
};

class RTC_EXPORT MultiplexAugmentOnlyDecoderFactory : public VideoDecoderFactory {
 public:
  // |supports_augmenting_data| defines if the decoder would support augmenting
  // data. If set, the decoder is expected to output video frame buffers of type
  // AugmentedVideoFrameBuffer.
  MultiplexAugmentOnlyDecoderFactory(std::unique_ptr<VideoDecoderFactory> factory,
                          bool supports_augmenting_data = false);

  std::vector<SdpVideoFormat> GetSupportedFormats() const override;
  std::unique_ptr<VideoDecoder> CreateVideoDecoder(
      const SdpVideoFormat& format) override;

 private:
  std::unique_ptr<VideoDecoderFactory> factory_;
  const bool supports_augmenting_data_;
};

}  // namespace webrtc

#endif  // MEDIA_ENGINE_MULTIPLEX_AUGMENT_ONLY_CODEC_FACTORY_H_
