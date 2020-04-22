/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import "RTCAugmentedVideoBuffer.h"
#import "RTCCVPixelBuffer.h"

#import "api/video_frame_buffer/RTCNativeMutableI420Buffer.h"

@implementation RTCAugmentedVideoBuffer

@synthesize augmentData = _augmentData;

+ (NSSet<NSNumber*>*)supportedPixelFormats {
  return [RTCCVPixelBuffer supportedPixelFormats];
}

- (instancetype)initWithPixelBuffer:(CVPixelBufferRef)pixelBuffer augmentData:(NSData*)augmentData {
  if (self = [super initWithPixelBuffer:pixelBuffer]) {
    _augmentData = augmentData;
  }

  return self;
}

- (instancetype)initWithPixelBuffer:(CVPixelBufferRef)pixelBuffer
                        augmentData:(NSData*)augmentData
                       adaptedWidth:(int)adaptedWidth
                      adaptedHeight:(int)adaptedHeight
                          cropWidth:(int)cropWidth
                         cropHeight:(int)cropHeight
                              cropX:(int)cropX
                              cropY:(int)cropY {
  if (self = [super initWithPixelBuffer:pixelBuffer 
                           adaptedWidth:adaptedWidth
                          adaptedHeight:adaptedHeight
                              cropWidth:cropWidth
                             cropHeight:cropHeight
                                  cropX:cropX
                                  cropY:cropY]) {
    _augmentData = augmentData;
  }

  return self;
}

@end
