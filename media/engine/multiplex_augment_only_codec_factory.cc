/*
 *  Copied and modified multiplex_codec_factory.cc
 */

#include "media/engine/multiplex_augment_only_codec_factory.h"

#include <map>
#include <string>
#include <utility>

#include "absl/strings/match.h"
#include "api/video_codecs/sdp_video_format.h"
#include "media/base/codec.h"
#include "media/base/media_constants.h"
#include "modules/video_coding/codecs/multiplex/include/multiplex_augment_only_decoder_adapter.h"
#include "modules/video_coding/codecs/multiplex/include/multiplex_augment_only_encoder_adapter.h"
#include "rtc_base/logging.h"

namespace {

bool IsMultiplexCodec(const cricket::VideoCodec& codec) {
  return absl::EqualsIgnoreCase(codec.name.c_str(),
                                cricket::kMultiplexCodecName);
}

}  // anonymous namespace

namespace webrtc {

constexpr const char* kMultiplexAssociatedCodecName = cricket::kH264CodecName;

MultiplexAugmentOnlyEncoderFactory::MultiplexAugmentOnlyEncoderFactory(
    std::unique_ptr<VideoEncoderFactory> factory,
    bool supports_augmenting_data)
    : factory_(std::move(factory)),
      supports_augmenting_data_(supports_augmenting_data) {}

std::vector<SdpVideoFormat> MultiplexAugmentOnlyEncoderFactory::GetSupportedFormats()
    const {
  std::vector<SdpVideoFormat> formats = factory_->GetSupportedFormats();
  std::vector<SdpVideoFormat> retFormats = std::vector<SdpVideoFormat>();
  for (const auto& format : formats) {
    if (absl::EqualsIgnoreCase(format.name, kMultiplexAssociatedCodecName)) {
      SdpVideoFormat multiplex_format = format;
      multiplex_format.parameters[cricket::kCodecParamAssociatedCodecName] =
          format.name;
      multiplex_format.name = cricket::kMultiplexCodecName;
      retFormats.push_back(multiplex_format);
      break;
    }
  }
  return retFormats;
}

VideoEncoderFactory::CodecInfo MultiplexAugmentOnlyEncoderFactory::QueryVideoEncoder(
    const SdpVideoFormat& format) const {
  if (!IsMultiplexCodec(cricket::VideoCodec(format)))
    return factory_->QueryVideoEncoder(format);
  return factory_->QueryVideoEncoder(
      SdpVideoFormat(kMultiplexAssociatedCodecName));
}

std::unique_ptr<VideoEncoder> MultiplexAugmentOnlyEncoderFactory::CreateVideoEncoder(
    const SdpVideoFormat& format) {
  if (!IsMultiplexCodec(cricket::VideoCodec(format)))
    return factory_->CreateVideoEncoder(format);
  const auto& it =
      format.parameters.find(cricket::kCodecParamAssociatedCodecName);
  if (it == format.parameters.end()) {
    RTC_LOG(LS_ERROR) << "No associated codec for multiplex.";
    return nullptr;
  }
  SdpVideoFormat associated_format = format;
  associated_format.name = it->second;
  return std::unique_ptr<VideoEncoder>(new MultiplexAugmentOnlyEncoderAdapter(
      factory_.get(), associated_format, supports_augmenting_data_));
}

MultiplexAugmentOnlyDecoderFactory::MultiplexAugmentOnlyDecoderFactory(
    std::unique_ptr<VideoDecoderFactory> factory,
    bool supports_augmenting_data)
    : factory_(std::move(factory)),
      supports_augmenting_data_(supports_augmenting_data) {}

std::vector<SdpVideoFormat> MultiplexAugmentOnlyDecoderFactory::GetSupportedFormats()
    const {
  std::vector<SdpVideoFormat> formats = factory_->GetSupportedFormats();
  std::vector<SdpVideoFormat> retFormats = std::vector<SdpVideoFormat>();
  for (const auto& format : formats) {
    if (absl::EqualsIgnoreCase(format.name, kMultiplexAssociatedCodecName)) {
      SdpVideoFormat multiplex_format = format;
      multiplex_format.parameters[cricket::kCodecParamAssociatedCodecName] =
          format.name;
      multiplex_format.name = cricket::kMultiplexCodecName;
      retFormats.push_back(multiplex_format);
    }
  }
  return retFormats;
}

std::unique_ptr<VideoDecoder> MultiplexAugmentOnlyDecoderFactory::CreateVideoDecoder(
    const SdpVideoFormat& format) {
  if (!IsMultiplexCodec(cricket::VideoCodec(format)))
    return factory_->CreateVideoDecoder(format);
  const auto& it =
      format.parameters.find(cricket::kCodecParamAssociatedCodecName);
  if (it == format.parameters.end()) {
    RTC_LOG(LS_ERROR) << "No assicated codec for multiplex.";
    return nullptr;
  }
  SdpVideoFormat associated_format = format;
  associated_format.name = it->second;
  return std::unique_ptr<VideoDecoder>(new MultiplexAugmentOnlyDecoderAdapter(
      factory_.get(), associated_format, supports_augmenting_data_));
}

}  // namespace webrtc
