/*
 *  Copied and modified multiplex_encoder_adapter.h
 */

#ifndef MODULES_VIDEO_CODING_CODECS_MULTIPLEX_INCLUDE_MULTIPLEX_AUGMENT_ONLY_ENCODER_ADAPTER_H_
#define MODULES_VIDEO_CODING_CODECS_MULTIPLEX_INCLUDE_MULTIPLEX_AUGMENT_ONLY_ENCODER_ADAPTER_H_

#include <map>
#include <memory>
#include <vector>

#include "api/fec_controller_override.h"
#include "api/video_codecs/sdp_video_format.h"
#include "api/video_codecs/video_encoder.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "modules/video_coding/codecs/multiplex/multiplex_encoded_image_packer.h"
#include "modules/video_coding/include/video_codec_interface.h"
#include "rtc_base/critical_section.h"
#include "modules/video_coding/codecs/multiplex/include/multiplex_encoder_adapter.h"

namespace webrtc {

class MultiplexAugmentOnlyEncoderAdapter : public VideoEncoder {
 public:
  // |factory| is not owned and expected to outlive this class.
  MultiplexAugmentOnlyEncoderAdapter(VideoEncoderFactory* factory,
                          const SdpVideoFormat& associated_format,
                          bool supports_augmenting_data = false);
  virtual ~MultiplexAugmentOnlyEncoderAdapter();

  // Implements VideoEncoder
  void SetFecControllerOverride(
      FecControllerOverride* fec_controller_override) override;
  int InitEncode(const VideoCodec* inst,
                 const VideoEncoder::Settings& settings) override;
  int Encode(const VideoFrame& input_image,
             const std::vector<VideoFrameType>* frame_types) override;
  int RegisterEncodeCompleteCallback(EncodedImageCallback* callback) override;
  void SetRates(const RateControlParameters& parameters) override;
  void OnPacketLossRateUpdate(float packet_loss_rate) override;
  void OnRttUpdate(int64_t rtt_ms) override;
  void OnLossNotification(const LossNotification& loss_notification) override;
  int Release() override;
  EncoderInfo GetEncoderInfo() const override;

  EncodedImageCallback::Result OnEncodedImage(
      const EncodedImage& encodedImage,
      const CodecSpecificInfo* codecSpecificInfo,
      const RTPFragmentationHeader* fragmentation);

 private:
  // Wrapper class that redirects OnEncodedImage() calls.
  class AdapterEncodedImageCallback;

  VideoEncoderFactory* const factory_;
  const SdpVideoFormat associated_format_;
  std::unique_ptr<VideoEncoder> encoder_;
  std::unique_ptr<AdapterEncodedImageCallback> adapter_callback_;
  EncodedImageCallback* encoded_complete_callback_;

  std::map<uint32_t /* timestamp */, MultiplexImage> stashed_images_
      RTC_GUARDED_BY(crit_);

  uint16_t picture_index_ = 0;

  rtc::CriticalSection crit_;

  const bool supports_augmented_data_;
  int augmenting_data_size_ = 0;

  EncoderInfo encoder_info_;
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_CODECS_MULTIPLEX_INCLUDE_MULTIPLEX_AUGMENT_ONLY_ENCODER_ADAPTER_H_
