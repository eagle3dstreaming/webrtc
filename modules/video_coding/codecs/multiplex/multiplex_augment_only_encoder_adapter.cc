/*
 *  Copied and modified multiplex_encoder_adapter.cc
 */

#include "modules/video_coding/codecs/multiplex/include/multiplex_augment_only_encoder_adapter.h"

#include <cstring>

#include "api/video/encoded_image.h"
#include "api/video_codecs/video_encoder.h"
#include "common_video/include/video_frame_buffer.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"
#include "media/base/video_common.h"
#include "modules/include/module_common_types.h"
#include "modules/video_coding/codecs/multiplex/include/augmented_video_frame_buffer.h"
#include "rtc_base/keep_ref_until_done.h"
#include "rtc_base/logging.h"

namespace webrtc {

// Callback wrapper that helps distinguish returned results from |encoders_|
// instances.
class MultiplexAugmentOnlyEncoderAdapter::AdapterEncodedImageCallback
    : public webrtc::EncodedImageCallback {
 public:
  AdapterEncodedImageCallback(webrtc::MultiplexAugmentOnlyEncoderAdapter* adapter)
      : adapter_(adapter) {}

  EncodedImageCallback::Result OnEncodedImage(
      const EncodedImage& encoded_image,
      const CodecSpecificInfo* codec_specific_info,
      const RTPFragmentationHeader* fragmentation) override {
    if (!adapter_)
      return Result(Result::OK);

    return adapter_->OnEncodedImage(encoded_image,
                                    codec_specific_info, fragmentation);
  }

 private:
  MultiplexAugmentOnlyEncoderAdapter* adapter_;
};

MultiplexAugmentOnlyEncoderAdapter::MultiplexAugmentOnlyEncoderAdapter(
    VideoEncoderFactory* factory,
    const SdpVideoFormat& associated_format,
    bool supports_augmented_data)
    : factory_(factory),
      associated_format_(associated_format),
      encoded_complete_callback_(nullptr),
      supports_augmented_data_(supports_augmented_data) {}

MultiplexAugmentOnlyEncoderAdapter::~MultiplexAugmentOnlyEncoderAdapter() {
  Release();
}

void MultiplexAugmentOnlyEncoderAdapter::SetFecControllerOverride(
    FecControllerOverride* fec_controller_override) {
  // Ignored.
}

int MultiplexAugmentOnlyEncoderAdapter::InitEncode(
    const VideoCodec* inst,
    const VideoEncoder::Settings& settings) {
  RTC_DCHECK_EQ(kVideoCodecMultiplex, inst->codecType);
  VideoCodec video_codec = *inst;
  video_codec.codecType = PayloadStringToCodecType(associated_format_.name);

  encoder_info_ = EncoderInfo();
  encoder_info_.implementation_name = "MultiplexAugmentOnlyEncoderAdapter (";
  encoder_info_.requested_resolution_alignment = 1;

  encoder_ = factory_->CreateVideoEncoder(associated_format_);
  const int rv = encoder_->InitEncode(&video_codec, settings);
  if (rv) {
    RTC_LOG(LS_ERROR) << "Failed to create multiplex codec.";
    return rv;
  }
  adapter_callback_ = std::unique_ptr<AdapterEncodedImageCallback>(new AdapterEncodedImageCallback(this));
  encoder_->RegisterEncodeCompleteCallback(adapter_callback_.get());

  const EncoderInfo& encoder_impl_info = encoder_->GetEncoderInfo();
  encoder_info_.implementation_name += encoder_impl_info.implementation_name;

  // Uses hardware support if any of the encoders uses it.
  // For example, if we are having issues with down-scaling due to
  // pipelining delay in HW encoders we need higher encoder usage
  // thresholds in CPU adaptation.
  encoder_info_.is_hardware_accelerated =
      encoder_impl_info.is_hardware_accelerated;

  encoder_info_.supports_native_handle = encoder_impl_info.supports_native_handle;

  encoder_info_.requested_resolution_alignment = cricket::LeastCommonMultiple(
      encoder_info_.requested_resolution_alignment,
      encoder_impl_info.requested_resolution_alignment);

  encoder_info_.has_internal_source = false;

  encoder_info_.implementation_name += ")";

  return WEBRTC_VIDEO_CODEC_OK;
}

int MultiplexAugmentOnlyEncoderAdapter::Encode(
    const VideoFrame& input_image,
    const std::vector<VideoFrameType>* frame_types) {
  if (!encoded_complete_callback_) {
    return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
  }

  std::unique_ptr<uint8_t[]> augmenting_data = nullptr;
  uint16_t augmenting_data_length = 0;
  AugmentedVideoFrameBuffer* augmented_video_frame_buffer = nullptr;
  if (supports_augmented_data_) {
    augmented_video_frame_buffer = static_cast<AugmentedVideoFrameBuffer*>(
        input_image.video_frame_buffer().get());
    augmenting_data_length =
        augmented_video_frame_buffer->GetAugmentingDataSize();
    augmenting_data =
        std::unique_ptr<uint8_t[]>(new uint8_t[augmenting_data_length]);
    memcpy(augmenting_data.get(),
           augmented_video_frame_buffer->GetAugmentingData(),
           augmenting_data_length);
    augmenting_data_size_ = augmenting_data_length;
  }

  {
    rtc::CritScope cs(&crit_);
    stashed_images_.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(input_image.timestamp()),
        std::forward_as_tuple(
            picture_index_, 1,
            std::move(augmenting_data), augmenting_data_length));
  }

  ++picture_index_;

  // Encode YUV
  int rv = encoder_->Encode(input_image, frame_types);
    
  return rv;
}

int MultiplexAugmentOnlyEncoderAdapter::RegisterEncodeCompleteCallback(
    EncodedImageCallback* callback) {
  encoded_complete_callback_ = callback;
  return WEBRTC_VIDEO_CODEC_OK;
}

void MultiplexAugmentOnlyEncoderAdapter::SetRates(
    const RateControlParameters& parameters) {
  VideoBitrateAllocation bitrate_allocation(parameters.bitrate);
  bitrate_allocation.SetBitrate(
      0, 0, parameters.bitrate.GetBitrate(0, 0) - augmenting_data_size_);
  // TODO(emircan): |framerate| is used to calculate duration in encoder
  // instances. We report the total frame rate to keep real time for now.
  // Remove this after refactoring duration logic.
  encoder_->SetRates(RateControlParameters(
      bitrate_allocation,
      static_cast<uint32_t>(parameters.framerate_fps),
      parameters.bandwidth_allocation -
          DataRate::bps(augmenting_data_size_)));
}

void MultiplexAugmentOnlyEncoderAdapter::OnPacketLossRateUpdate(float packet_loss_rate) {
  encoder_->OnPacketLossRateUpdate(packet_loss_rate);
}

void MultiplexAugmentOnlyEncoderAdapter::OnRttUpdate(int64_t rtt_ms) {
  encoder_->OnRttUpdate(rtt_ms);
}

void MultiplexAugmentOnlyEncoderAdapter::OnLossNotification(
    const LossNotification& loss_notification) {
  encoder_->OnLossNotification(loss_notification);
}

int MultiplexAugmentOnlyEncoderAdapter::Release() {
  const int rv = encoder_->Release();
  if (rv)
    return rv;

  rtc::CritScope cs(&crit_);
  stashed_images_.clear();

  return WEBRTC_VIDEO_CODEC_OK;
}

VideoEncoder::EncoderInfo MultiplexAugmentOnlyEncoderAdapter::GetEncoderInfo() const {
  return encoder_info_;
}

EncodedImageCallback::Result MultiplexAugmentOnlyEncoderAdapter::OnEncodedImage(
    const EncodedImage& encodedImage,
    const CodecSpecificInfo* codecSpecificInfo,
    const RTPFragmentationHeader* fragmentation) {
  // Save the image
  MultiplexImageComponent image_component;
  image_component.component_index = 1;
  image_component.codec_type =
      PayloadStringToCodecType(associated_format_.name);
  image_component.encoded_image = encodedImage;

  // If we don't already own the buffer, make a copy.
  image_component.encoded_image.Retain();

  rtc::CritScope cs(&crit_);
  const auto& stashed_image_itr =
      stashed_images_.find(encodedImage.Timestamp());
  const auto& stashed_image_next_itr = std::next(stashed_image_itr, 1);
  RTC_DCHECK(stashed_image_itr != stashed_images_.end());
  MultiplexImage& stashed_image = stashed_image_itr->second;

  stashed_image.image_components.push_back(image_component);

  CodecSpecificInfo codec_info = *codecSpecificInfo;
  codec_info.codecType = kVideoCodecMultiplex;
  encoded_complete_callback_->OnEncodedImage(MultiplexEncodedImagePacker::PackAndRelease(stashed_image),
                                              &codec_info,
                                              fragmentation);

  stashed_images_.erase(stashed_images_.begin(), stashed_image_next_itr);

  return EncodedImageCallback::Result(EncodedImageCallback::Result::OK);
}

}  // namespace webrtc
