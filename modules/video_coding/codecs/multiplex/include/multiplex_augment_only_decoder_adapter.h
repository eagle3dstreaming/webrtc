/*
 *  Copied and modified multiplex_decoder_adapter.h
 */

#ifndef MODULES_VIDEO_CODING_CODECS_MULTIPLEX_INCLUDE_MULTIPLEX_AUGMENT_ONLY_DECODER_ADAPTER_H_
#define MODULES_VIDEO_CODING_CODECS_MULTIPLEX_INCLUDE_MULTIPLEX_AUGMENT_ONLY_DECODER_ADAPTER_H_

#include <map>
#include <memory>
#include <vector>

#include "api/video_codecs/sdp_video_format.h"
#include "api/video_codecs/video_decoder.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "modules/video_coding/codecs/multiplex/include/multiplex_encoder_adapter.h"

namespace webrtc {

class MultiplexAugmentOnlyDecoderAdapter : public VideoDecoder {
 public:
  // |factory| is not owned and expected to outlive this class.
  MultiplexAugmentOnlyDecoderAdapter(VideoDecoderFactory* factory,
                          const SdpVideoFormat& associated_format,
                          bool supports_augmenting_data = false);
  virtual ~MultiplexAugmentOnlyDecoderAdapter();

  // Implements VideoDecoder
  int32_t InitDecode(const VideoCodec* codec_settings,
                     int32_t number_of_cores) override;
  int32_t Decode(const EncodedImage& input_image,
                 bool missing_frames,
                 int64_t render_time_ms) override;
  int32_t RegisterDecodeCompleteCallback(
      DecodedImageCallback* callback) override;
  int32_t Release() override;

  void Decoded(VideoFrame* decoded_image,
               absl::optional<int32_t> decode_time_ms,
               absl::optional<uint8_t> qp);

 private:
  // Wrapper class that redirects Decoded() calls.
  class AdapterDecodedImageCallback;

  // Holds the augmenting data of an image
  struct AugmentingData;

  void MergeAugmentedImages(VideoFrame* decoded_image,
                        const absl::optional<int32_t>& decode_time_ms,
                        const absl::optional<uint8_t>& qp,
                        std::unique_ptr<uint8_t[]> augmenting_data,
                        uint16_t augmenting_data_length);

  VideoDecoderFactory* const factory_;
  const SdpVideoFormat associated_format_;
  std::unique_ptr<VideoDecoder> decoder_;
  std::unique_ptr<AdapterDecodedImageCallback> adapter_callback_;
  DecodedImageCallback* decoded_complete_callback_;

  std::map<uint32_t /* timestamp */, AugmentingData> decoded_augmenting_data_;
  const bool supports_augmenting_data_;
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_CODECS_MULTIPLEX_INCLUDE_MULTIPLEX_AUGMENT_ONLY_DECODER_ADAPTER_H_
