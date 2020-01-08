/*
 *  Copyright 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "pc/local_audio_source.h"

#include "rtc_base/ref_counted_object.h"

#include <vector>

#include "rtc_base/thread.h"

using webrtc::MediaSourceInterface;

namespace webrtc {

// This proxy is passed to the underlying media engine to receive audio data as
// they come in. The data will then be passed back up to the LocalAudioSource
// which will fan it out to all the sinks that have been added to it.
class LocalAudioSource::AudioDataProxy : public AudioSinkInterface {
 public:
  explicit AudioDataProxy(LocalAudioSource* source) : source_(source) {
    RTC_DCHECK(source);
  }
  ~AudioDataProxy() override { source_->OnAudioChannelGone(); }

  // AudioSinkInterface implementation.
  void OnData(const AudioSinkInterface::Data& audio) override {
    source_->OnData(audio);
  }

 private:
  const rtc::scoped_refptr<LocalAudioSource> source_;

  RTC_DISALLOW_IMPLICIT_CONSTRUCTORS(AudioDataProxy);
};

rtc::scoped_refptr<LocalAudioSource> LocalAudioSource::Create(
    rtc::Thread* worker_thread,
    const cricket::AudioOptions* audio_options) {
  rtc::scoped_refptr<LocalAudioSource> source(
      new rtc::RefCountedObject<LocalAudioSource>(worker_thread));
  source->Initialize(audio_options);
  return source;
}

void LocalAudioSource::Initialize(const cricket::AudioOptions* audio_options) {
  if (!audio_options)
    return;

  options_ = *audio_options;
}

void LocalAudioSource::Start(cricket::VoiceMediaChannel* media_channel,
                             uint32_t ssrc) {
  RTC_DCHECK(media_channel);
  // Register for callbacks immediately before AddSink so that we always get
  // notified when a channel goes out of scope (signaled when "AudioDataProxy"
  // is destroyed).
  worker_thread_->Invoke<void>(RTC_FROM_HERE, [&] {
    media_channel->SetRawAudioSink(ssrc,
                                   absl::make_unique<AudioDataProxy>(this));
  });
}

void LocalAudioSource::Stop(cricket::VoiceMediaChannel* media_channel,
                            uint32_t ssrc) {
  RTC_DCHECK(media_channel);
  worker_thread_->Invoke<void>(
      RTC_FROM_HERE, [&] { media_channel->SetRawAudioSink(ssrc, nullptr); });
}

void LocalAudioSource::AddSink(AudioTrackSinkInterface* sink) {
  RTC_DCHECK(sink);
  rtc::CritScope lock(&sink_lock_);
  RTC_DCHECK(std::find(sinks_.begin(), sinks_.end(), sink) == sinks_.end());
  sinks_.push_back(sink);
}

void LocalAudioSource::RemoveSink(AudioTrackSinkInterface* sink) {
  RTC_DCHECK(sink);

  rtc::CritScope lock(&sink_lock_);
  sinks_.remove(sink);
}

void LocalAudioSource::OnData(const AudioSinkInterface::Data& audio) {
  // Called on the externally-owned audio callback thread, via/from webrtc.
  rtc::CritScope lock(&sink_lock_);
  for (auto* sink : sinks_) {
    sink->OnData(audio.data, 16, audio.sample_rate, audio.channels,
                 audio.samples_per_channel);
  }
}

void LocalAudioSource::OnAudioChannelGone() {}

}  // namespace webrtc
