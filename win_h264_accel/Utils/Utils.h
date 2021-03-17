

#ifndef _UTILS_UTILS_H_
#define _UTILS_UTILS_H_

//#define ON_SUCCEEDED(act) SUCCEEDED(hr) && SUCCEEDED(hr = act)
#define ON_SUCCEEDED(act) if (SUCCEEDED(hr)) { hr = act; if (FAILED(hr)) { RTC_LOG(LS_WARNING) << "ERROR:" << #act; } }

#endif  // _UTILS_UTILS_H_
