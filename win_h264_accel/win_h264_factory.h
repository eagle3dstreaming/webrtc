
#ifndef _H264_WINUWP_FACTORY_H_
#define _H264_WINUWP_FACTORY_H_

#include <vector>
#include "api/video_codecs/video_encoder_factory.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "media/base/codec.h"

namespace webrtc {

class WinH264EncoderFactory : public webrtc::VideoEncoderFactory {
 public:
  WinH264EncoderFactory();

  std::vector<SdpVideoFormat> GetSupportedFormats() const override;

  CodecInfo QueryVideoEncoder(const SdpVideoFormat& format) const override;

  std::unique_ptr<VideoEncoder> CreateVideoEncoder(
    const SdpVideoFormat& format) override;
};

class WinUWPH264DecoderFactory : public webrtc::VideoDecoderFactory {

  std::vector<SdpVideoFormat> GetSupportedFormats() const override;

  std::unique_ptr<VideoDecoder> CreateVideoDecoder(
      const SdpVideoFormat& format) override;
};

}  // namespace webrtc

#endif  // _H264_WINUWP_FACTORY_H_
