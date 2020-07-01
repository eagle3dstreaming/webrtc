/*
 *  Copied and modified multiplex_decoder_adapter.cc
 */

#include "modules/video_coding/codecs/multiplex/include/multiplex_augment_only_decoder_adapter.h"

#include "api/video/encoded_image.h"
#include "api/video/i420_buffer.h"
#include "api/video/video_frame_buffer.h"
#include "common_video/include/video_frame_buffer.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"
#include "modules/video_coding/codecs/multiplex/include/augmented_video_frame_buffer.h"
#include "modules/video_coding/codecs/multiplex/multiplex_encoded_image_packer.h"
#include "rtc_base/keep_ref_until_done.h"
#include "rtc_base/logging.h"

namespace webrtc {

class MultiplexAugmentOnlyDecoderAdapter::AdapterDecodedImageCallback
    : public webrtc::DecodedImageCallback {
 public:
  AdapterDecodedImageCallback(webrtc::MultiplexAugmentOnlyDecoderAdapter* adapter)
      : adapter_(adapter) {}

  void Decoded(VideoFrame& decoded_image,
               absl::optional<int32_t> decode_time_ms,
               absl::optional<uint8_t> qp) override {
    if (!adapter_)
      return;
    adapter_->Decoded(&decoded_image, decode_time_ms, qp);
  }
  int32_t Decoded(VideoFrame& decoded_image) override {
    adapter_->Decoded(&decoded_image, absl::nullopt, absl::nullopt);
    return WEBRTC_VIDEO_CODEC_OK;
  }
  int32_t Decoded(VideoFrame& decoded_image, int64_t decode_time_ms) override {
    adapter_->Decoded(&decoded_image, decode_time_ms, absl::nullopt);
    return WEBRTC_VIDEO_CODEC_OK;
  }

 private:
  MultiplexAugmentOnlyDecoderAdapter* adapter_;
};

struct MultiplexAugmentOnlyDecoderAdapter::AugmentingData {
  AugmentingData(std::unique_ptr<uint8_t[]> augmenting_data, uint16_t data_size)
      : data_(std::move(augmenting_data)), size_(data_size) {}
  std::unique_ptr<uint8_t[]> data_;
  const uint16_t size_;

 private:
  RTC_DISALLOW_IMPLICIT_CONSTRUCTORS(AugmentingData);
};

MultiplexAugmentOnlyDecoderAdapter::MultiplexAugmentOnlyDecoderAdapter(
    VideoDecoderFactory* factory,
    const SdpVideoFormat& associated_format,
    bool supports_augmenting_data)
    : factory_(factory),
      associated_format_(associated_format),
      supports_augmenting_data_(supports_augmenting_data) {}

MultiplexAugmentOnlyDecoderAdapter::~MultiplexAugmentOnlyDecoderAdapter() {
  Release();
}

int32_t MultiplexAugmentOnlyDecoderAdapter::InitDecode(const VideoCodec* codec_settings,
                                            int32_t number_of_cores) {
  RTC_DCHECK_EQ(kVideoCodecMultiplex, codec_settings->codecType);
  VideoCodec settings = *codec_settings;
  settings.codecType = PayloadStringToCodecType(associated_format_.name);
  decoder_ = factory_->CreateVideoDecoder(associated_format_);

  const int32_t rv = decoder_->InitDecode(&settings, number_of_cores);
  if (rv)
    return rv;
  adapter_callback_ = 
    std::unique_ptr<MultiplexAugmentOnlyDecoderAdapter::AdapterDecodedImageCallback>(
      new MultiplexAugmentOnlyDecoderAdapter::AdapterDecodedImageCallback(this));
  decoder_->RegisterDecodeCompleteCallback(adapter_callback_.get());
  return WEBRTC_VIDEO_CODEC_OK;
}

int32_t MultiplexAugmentOnlyDecoderAdapter::Decode(const EncodedImage& input_image,
                                        bool missing_frames,
                                        int64_t render_time_ms) {
  MultiplexImage image = MultiplexEncodedImagePacker::Unpack(input_image);

  if (supports_augmenting_data_) {
    RTC_DCHECK(decoded_augmenting_data_.find(input_image.Timestamp()) ==
               decoded_augmenting_data_.end());
    decoded_augmenting_data_.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(input_image.Timestamp()),
        std::forward_as_tuple(std::move(image.augmenting_data),
                              image.augmenting_data_size));
  }

  int32_t rv = decoder_->Decode(
      image.image_components[0].encoded_image, missing_frames,
      render_time_ms);
  if (rv != WEBRTC_VIDEO_CODEC_OK)
    return rv;

  return rv;
}

int32_t MultiplexAugmentOnlyDecoderAdapter::RegisterDecodeCompleteCallback(
    DecodedImageCallback* callback) {
  decoded_complete_callback_ = callback;
  return WEBRTC_VIDEO_CODEC_OK;
}

int32_t MultiplexAugmentOnlyDecoderAdapter::Release() {
  if(decoder_ != nullptr)
  {
  const int32_t rv = decoder_->Release();
  if (rv)
    return rv;
  }
  return WEBRTC_VIDEO_CODEC_OK;
}

void MultiplexAugmentOnlyDecoderAdapter::Decoded(VideoFrame* decoded_image,
                                      absl::optional<int32_t> decode_time_ms,
                                      absl::optional<uint8_t> qp) {
  const auto& augmenting_data_it =
      decoded_augmenting_data_.find(decoded_image->timestamp());
  const bool has_augmenting_data =
      augmenting_data_it != decoded_augmenting_data_.end();
    
  uint16_t augmenting_data_size =
      has_augmenting_data ? augmenting_data_it->second.size_ : 0;
  std::unique_ptr<uint8_t[]> augmenting_data =
      has_augmenting_data ? std::move(augmenting_data_it->second.data_)
                          : nullptr;
      
  MergeAugmentedImages(decoded_image, decode_time_ms, qp,
                  std::move(augmenting_data), augmenting_data_size);

  if (has_augmenting_data) {
    decoded_augmenting_data_.erase(decoded_augmenting_data_.begin(),
                                  std::next(augmenting_data_it, 1));
  }
}

void MultiplexAugmentOnlyDecoderAdapter::MergeAugmentedImages(
    VideoFrame* decoded_image,
    const absl::optional<int32_t>& decode_time_ms,
    const absl::optional<uint8_t>& qp,
    std::unique_ptr<uint8_t[]> augmenting_data,
    uint16_t augmenting_data_length) {
  rtc::scoped_refptr<VideoFrameBuffer> merged_buffer = decoded_image->video_frame_buffer();
  if (supports_augmenting_data_) {
    merged_buffer = rtc::scoped_refptr<webrtc::AugmentedVideoFrameBuffer>(
        new rtc::RefCountedObject<AugmentedVideoFrameBuffer>(
            merged_buffer, std::move(augmenting_data), augmenting_data_length));
  }

  VideoFrame merged_image = VideoFrame::Builder()
                                .set_video_frame_buffer(merged_buffer)
                                .set_timestamp_rtp(decoded_image->timestamp())
                                .set_timestamp_us(0)
                                .set_rotation(decoded_image->rotation())
                                .set_id(decoded_image->id())
                                .set_packet_infos(decoded_image->packet_infos())
                                .build();
  decoded_complete_callback_->Decoded(merged_image, decode_time_ms, qp);
}

}  // namespace webrtc
