/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import <Foundation/Foundation.h>

#import <WebRTC/RTCMacros.h>

NS_ASSUME_NONNULL_BEGIN

@class RTCAudioData;

RTC_OBJC_EXPORT
@protocol RTCAudioSink <NSObject>

/** The audio data available. */
- (void)onData:(const void*)audioData
       bitsPerSample:(int)bitsPerSample
          sampleRate:(int)sampleRate
    numberOfChannels:(size_t)numberOfChannels
      numberOfFrames:(size_t)numberOfFrames;

@end

NS_ASSUME_NONNULL_END
