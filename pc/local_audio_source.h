/*
 *  Copyright 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_LOCAL_AUDIO_SOURCE_H_
#define PC_LOCAL_AUDIO_SOURCE_H_

#include <list>

#include "api/call/audio_sink.h"
#include "api/audio_options.h"
#include "api/media_stream_interface.h"
#include "api/notifier.h"
#include "api/scoped_refptr.h"
#include "rtc_base/critical_section.h"
#include "media/base/media_channel.h"


// LocalAudioSource implements AudioSourceInterface.
// This contains settings for switching audio processing on and off.

namespace rtc {
class Thread;
}  // namespace rtc

namespace webrtc {

class LocalAudioSource : public Notifier<AudioSourceInterface> {
 public:
  // Creates an instance of LocalAudioSource.
  static rtc::scoped_refptr<LocalAudioSource> Create(
      rtc::Thread* worker_thread,
      const cricket::AudioOptions* audio_options);

  SourceState state() const override { return kLive; }
  bool remote() const override { return false; }
    
  // Register and unregister remote audio source with the underlying media
  // engine.
  void Start(cricket::VoiceMediaChannel* media_channel, uint32_t ssrc);
  void Stop(cricket::VoiceMediaChannel* media_channel, uint32_t ssrc);

  const cricket::AudioOptions options() const override { return options_; }

  void AddSink(AudioTrackSinkInterface* sink) override;
  void RemoveSink(AudioTrackSinkInterface* sink) override;

 protected:
  explicit LocalAudioSource(rtc::Thread* worker_thread)
      : worker_thread_(worker_thread) {}
  ~LocalAudioSource() override {}

 private:
  void Initialize(const cricket::AudioOptions* audio_options);

  // These are callbacks from the media engine.
  class AudioDataProxy;
  void OnData(const AudioSinkInterface::Data& audio);
  void OnAudioChannelGone();

  cricket::AudioOptions options_;
  rtc::Thread* worker_thread_;
  rtc::CriticalSection sink_lock_;
  std::list<AudioTrackSinkInterface*> sinks_;
};

}  // namespace webrtc

#endif  // PC_LOCAL_AUDIO_SOURCE_H_
