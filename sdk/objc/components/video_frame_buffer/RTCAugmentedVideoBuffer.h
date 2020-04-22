#import "RTCCVPixelBuffer.h"

NS_ASSUME_NONNULL_BEGIN

@class RTCCVPixelBuffer;

/** RTCVideoFrameBuffer containing a CVPixelBufferRef */
RTC_OBJC_EXPORT
@interface RTCAugmentedVideoBuffer : RTCCVPixelBuffer

@property(nonatomic, readonly) NSData* augmentData;

- (instancetype)initWithPixelBuffer:(CVPixelBufferRef)pixelBuffer augmentData:(NSData*)augmentData;

- (instancetype)initWithPixelBuffer:(CVPixelBufferRef)pixelBuffer
                        augmentData:(NSData*)augmentData
                       adaptedWidth:(int)adaptedWidth
                      adaptedHeight:(int)adaptedHeight
                          cropWidth:(int)cropWidth
                         cropHeight:(int)cropHeight
                              cropX:(int)cropX
                              cropY:(int)cropY;

@end

NS_ASSUME_NONNULL_END
