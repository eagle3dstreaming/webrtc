

#ifndef H264ENCODER_IH264ENCODINGCALLBACK_H_
#define H264ENCODER_IH264ENCODINGCALLBACK_H_

#include <wrl\implements.h>


namespace webrtc {

interface IH264EncodingCallback {
    virtual void OnH264Encoded(Microsoft::WRL::ComPtr<IMFSample> sample) = 0;
};

}  // namespace webrtc

#endif  // H264ENCODER_IH264ENCODINGCALLBACK_H_

