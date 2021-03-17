

#ifndef _H264ENCODER_H264MEDIASINK_H_
#define _H264ENCODER_H264MEDIASINK_H_

#include <Mferror.h>
#include <mfidl.h>
#include <windows.foundation.h>
#include <windows.media.h>
#include <windows.media.mediaproperties.h>
#include <wrl.h>

#include "../Utils/CritSec.h"
#include "IH264EncodingCallback.h"
#include "H264StreamSink.h"

namespace webrtc {

class H264StreamSink;
class H264MediaSink : public Microsoft::WRL::RuntimeClass<
  Microsoft::WRL::RuntimeClassFlags<
  Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
  ABI::Windows::Media::IMediaExtension,
  Microsoft::WRL::FtmBase,
  IMFMediaSink,
  IMFClockStateSink> {
  InspectableClass(L"H264MediaSink", BaseTrust)

 public:
  H264MediaSink();
  virtual ~H264MediaSink();

  HRESULT RuntimeClassInitialize();

  // IMediaExtension
  IFACEMETHOD(SetProperties)
    (ABI::Windows::Foundation::Collections::IPropertySet
      *pConfiguration) {
    return S_OK;
  }

  // IMFMediaSink methods
  IFACEMETHOD(GetCharacteristics) (DWORD *pdwCharacteristics);

  IFACEMETHOD(AddStreamSink)(
    /* [in] */ DWORD dwStreamSinkIdentifier,
    /* [in] */ IMFMediaType *pMediaType,
    /* [out] */ IMFStreamSink **ppStreamSink);

  IFACEMETHOD(RemoveStreamSink) (DWORD dwStreamSinkIdentifier);
  IFACEMETHOD(GetStreamSinkCount) (_Out_ DWORD *pcStreamSinkCount);
  IFACEMETHOD(GetStreamSinkByIndex)
    (DWORD dwIndex, _Outptr_ IMFStreamSink **ppStreamSink);
  IFACEMETHOD(GetStreamSinkById)
    (DWORD dwIdentifier, IMFStreamSink **ppStreamSink);
  IFACEMETHOD(SetPresentationClock)
    (IMFPresentationClock *pPresentationClock);
  IFACEMETHOD(GetPresentationClock)
    (IMFPresentationClock **ppPresentationClock);
  IFACEMETHOD(Shutdown) ();

  // IMFClockStateSink methods
  IFACEMETHOD(OnClockStart)
    (MFTIME hnsSystemTime, LONGLONG llClockStartOffset);
  IFACEMETHOD(OnClockStop) (MFTIME hnsSystemTime);
  IFACEMETHOD(OnClockPause) (MFTIME hnsSystemTime);
  IFACEMETHOD(OnClockRestart) (MFTIME hnsSystemTime);
  IFACEMETHOD(OnClockSetRate) (MFTIME hnsSystemTime, float flRate);

  HRESULT RegisterEncodingCallback(IH264EncodingCallback *callback);

 private:
  void HandleError(HRESULT hr);

  HRESULT CheckShutdown() const {
    if (isShutdown_) {
      return MF_E_SHUTDOWN;
    } else {
      return S_OK;
    }
  }

 private:
  CritSec critSec_;

  bool isShutdown_;

  Microsoft::WRL::ComPtr<IMFPresentationClock> spClock_;
  Microsoft::WRL::ComPtr<H264StreamSink> outputStream_;
};

}  // namespace webrtc

#endif  // _H264ENCODER_H264MEDIASINK_H_
