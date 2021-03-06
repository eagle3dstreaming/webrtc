

#ifndef _H264ENCODER_H264ENCODER_H_
#define _H264ENCODER_H264ENCODER_H_

#include <mfapi.h>
#include <mfidl.h>
#include <Mfreadwrite.h>
#include <mferror.h>
#include <vector>
#include "H264MediaSink.h"
#include "IH264EncodingCallback.h"
#include "../Utils/SampleAttributeQueue.h"
#include "api/video_codecs/video_encoder.h"
#include "rtc_base/critical_section.h"
#include "modules/video_coding/utility/quality_scaler.h"
#include "common_video/h264/h264_bitstream_parser.h"

#pragma comment(lib, "mfreadwrite")
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")

namespace webrtc {

class H264MediaSink;

class WinUWPH264EncoderImpl : public VideoEncoder, public IH264EncodingCallback {
 public:
  WinUWPH264EncoderImpl();

  ~WinUWPH264EncoderImpl();

  // === VideoEncoder overrides ===
  int InitEncode(const VideoCodec* codec_settings,
    int number_of_cores, size_t max_payload_size) override;
  int RegisterEncodeCompleteCallback(EncodedImageCallback* callback) override;
  int Release() override;
  int32_t Encode(const VideoFrame& frame,
                 const std::vector<VideoFrameType>* frame_types) override;
  //int32_t SetRateAllocation(const VideoBitrateAllocation& allocation,
   //                                 uint32_t framerate) override;
  void SetRates(const RateControlParameters& parameters) override;

  // === IH264EncodingCallback overrides ===
  void OnH264Encoded(ComPtr<IMFSample> sample) override;

 private:
  ComPtr<IMFSample> FromVideoFrame(const VideoFrame& frame);
  int InitEncoderWithSettings(const VideoCodec* codec_settings);

 private:
  rtc::CriticalSection crit_;
  rtc::CriticalSection callbackCrit_;
  bool inited_ {};
  ComPtr<IMFSinkWriter> sinkWriter_;
  ComPtr<IMFAttributes> sinkWriterCreationAttributes_;
  ComPtr<IMFAttributes> sinkWriterEncoderAttributes_;
  ComPtr<H264MediaSink> mediaSink_;
  EncodedImageCallback* encodedCompleteCallback_ {};
  DWORD streamIndex_ {};
  LONGLONG startTime_ {};
  LONGLONG lastTimestampHns_ {};
  bool firstFrame_ {true};
  int framePendingCount_ {};
  DWORD frameCount_ {};
  bool lastFrameDropped_ {};
  //These fields are never used
  /*
  UINT32 currentWidth_ {};
  UINT32 currentHeight_ {};
  UINT32 currentBitrateBps_ {};
  UINT32 currentFps_ {};
  */
  UINT32 max_bitrate_;

  UINT32 width_;
  UINT32 height_;
  UINT32 max_frame_rate_;
  UINT32 target_bps_;
  VideoCodecMode mode_;  
  // H.264 specifc parameters
  bool frame_dropping_on_;
  int key_frame_interval_;

  int64_t lastTimeSettingsChanged_ {};

  struct CachedFrameAttributes {
    uint32_t timestamp;
    uint64_t ntpTime;
    uint64_t captureRenderTime;
    uint32_t frameWidth;
    uint32_t frameHeight;
  };
  SampleAttributeQueue<CachedFrameAttributes> _sampleAttributeQueue;

  // Caching the codec received in InitEncode().
  VideoCodec codec_;
};  // end of WinUWPH264EncoderImpl class

}  // namespace webrtc
#endif  // _H264ENCODER_H264ENCODER_H_

