
#ifndef _UTILS_SAMPLEATTRIBUTEQUEUE_H_
#define _UTILS_SAMPLEATTRIBUTEQUEUE_H_

#include <stdint.h>
#include <queue>
#include <utility>
#include <memory>
#include "rtc_base/critical_section.h"

// A sorted queue with certain properties which makes it
// good for mapping attributes to frames and samples.
// The ids have to be in increasing order.
template <typename T>
class SampleAttributeQueue {
 public:
  SampleAttributeQueue()
  {
  }
  ~SampleAttributeQueue() {}

  void push(uint64_t id, const T& t) {
    rtc::CritScope lock(&_crit);
    _attributes.push(std::make_pair(id, t));
  }

  bool pop(uint64_t id, T& outT) {
    rtc::CritScope lock(&_crit);
    while (!_attributes.empty()) {
      auto entry = _attributes.front();
      if (entry.first > id) {
        outT = entry.second;
        return true;
      } else if (entry.first == id) {
        outT = entry.second;
        _attributes.pop();
        return true;
      } else {
        _attributes.pop();
      }
    }
    return false;
  }

  void clear() {
    rtc::CritScope lock(&_crit);
    while (!_attributes.empty()) {
      _attributes.pop();
    }
  }

  uint32_t size() {
    rtc::CritScope lock(&_crit);
    return static_cast<uint32_t>(_attributes.size());
  }

private:
  rtc::CriticalSection _crit;
  std::queue<std::pair<uint64_t, const T>> _attributes;
};

#endif  // _UTILS_SAMPLEATTRIBUTEQUEUE_H_
