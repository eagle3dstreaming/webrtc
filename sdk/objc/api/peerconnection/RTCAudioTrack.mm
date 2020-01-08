/*
 *  Copyright 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import "RTCAudioTrack+Private.h"

#import "RTCAudioSink.h"
#import "RTCAudioSource+Private.h"
#import "RTCMediaStreamTrack+Private.h"
#import "RTCPeerConnectionFactory+Private.h"
#import "helpers/NSString+StdString.h"

#include "api/media_stream_interface.h"
#include "rtc_base/checks.h"

class AudioSinkAdapter;

@interface RTCAudioSinkAdapter : NSObject

@property(nonatomic, readonly) id<RTCAudioSink> audioSink;

- (instancetype)init NS_UNAVAILABLE;

@end

class AudioSinkAdapter : public webrtc::AudioTrackSinkInterface {
 public:
  AudioSinkAdapter(RTCAudioSinkAdapter *adapter) { adapter_ = adapter; }
  void OnData(const void *audio_data,
              int bits_per_sample,
              int sample_rate,
              size_t number_of_channels,
              size_t number_of_frames) override {
    [adapter_.audioSink onData:audio_data
                 bitsPerSample:bits_per_sample
                    sampleRate:sample_rate
              numberOfChannels:number_of_channels
                numberOfFrames:number_of_frames];
  }

 private:
  __weak RTCAudioSinkAdapter *adapter_;
};

@implementation RTCAudioSinkAdapter {
  std::unique_ptr<AudioSinkAdapter> _adapter;
}

@synthesize audioSink = _audioSink;

- (instancetype)initWithNativeSink:(id<RTCAudioSink>)audioSink {
  NSParameterAssert(audioSink);
  if (self = [super init]) {
    _audioSink = audioSink;
    _adapter.reset(new AudioSinkAdapter(self));
  }
  return self;
}

- (AudioSinkAdapter *)nativeAudioSink {
  return _adapter.get();
}

@end

@implementation RTCAudioTrack {
  NSMutableArray *_adapters;
}

@synthesize source = _source;

- (instancetype)initWithFactory:(RTCPeerConnectionFactory *)factory
                         source:(RTCAudioSource *)source
                        trackId:(NSString *)trackId {
  RTC_DCHECK(factory);
  RTC_DCHECK(source);
  RTC_DCHECK(trackId.length);

  std::string nativeId = [NSString stdStringForString:trackId];
  rtc::scoped_refptr<webrtc::AudioTrackInterface> track =
      factory.nativeFactory->CreateAudioTrack(nativeId, source.nativeAudioSource);
  if (self = [self initWithFactory:factory nativeTrack:track type:RTCMediaStreamTrackTypeAudio]) {
    _source = source;
  }
  return self;
}

- (instancetype)initWithFactory:(RTCPeerConnectionFactory *)factory
                    nativeTrack:(rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>)nativeTrack
                           type:(RTCMediaStreamTrackType)type {
  NSParameterAssert(factory);
  NSParameterAssert(nativeTrack);
  NSParameterAssert(type == RTCMediaStreamTrackTypeAudio);
  if (self = [super initWithFactory:factory nativeTrack:nativeTrack type:type]) {
    _adapters = [NSMutableArray array];
  }
  return self;
}

- (void)dealloc {
  for (RTCAudioSinkAdapter *adapter in _adapters) {
    self.nativeAudioTrack->RemoveSink(adapter.nativeAudioSink);
  }
}


- (RTCAudioSource *)source {
  if (!_source) {
    rtc::scoped_refptr<webrtc::AudioSourceInterface> source =
        self.nativeAudioTrack->GetSource();
    if (source) {
      _source =
          [[RTCAudioSource alloc] initWithFactory:self.factory nativeAudioSource:source.get()];
    }
  }
  return _source;
}

- (void)addSink:(id<RTCAudioSink>)sink {
  // Make sure we don't have this renderer yet.
  for (RTCAudioSinkAdapter *adapter in _adapters) {
    if (adapter.audioSink == sink) {
      NSAssert(NO, @"|sink| is already attached to this track");
      return;
    }
  }
  // Create a wrapper that provides a native pointer for us.
  RTCAudioSinkAdapter *adapter = [[RTCAudioSinkAdapter alloc] initWithNativeSink:sink];
  [_adapters addObject:adapter];

  rtc::scoped_refptr<webrtc::AudioTrackInterface> audioTrack =
      static_cast<webrtc::AudioTrackInterface *>(self.nativeAudioTrack.get());
  audioTrack->AddSink(adapter.nativeAudioSink);
}

- (void)removeSink:(id<RTCAudioSink>)sink {
  __block NSUInteger indexToRemove = NSNotFound;
  [_adapters
      enumerateObjectsUsingBlock:^(RTCAudioSinkAdapter *adapter, NSUInteger idx, BOOL *stop) {
        if (adapter.audioSink == sink) {
          indexToRemove = idx;
          *stop = YES;
        }
      }];
  if (indexToRemove == NSNotFound) {
    return;
  }
  RTCAudioSinkAdapter *adapterToRemove = [_adapters objectAtIndex:indexToRemove];
  self.nativeAudioTrack->RemoveSink(adapterToRemove.nativeAudioSink);
  [_adapters removeObjectAtIndex:indexToRemove];
}

#pragma mark - Private

- (rtc::scoped_refptr<webrtc::AudioTrackInterface>)nativeAudioTrack {
  return static_cast<webrtc::AudioTrackInterface *>(self.nativeTrack.get());
}

@end
