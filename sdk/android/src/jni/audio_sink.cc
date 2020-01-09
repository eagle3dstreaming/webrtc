/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "sdk/android/src/jni/audiosink.h"
#include "sdk/android/generated_video_jni/jni/AudioSink_jni.h"

namespace webrtc {
namespace jni {

AudioSinkWrapper::AudioSinkWrapper(JNIEnv* jni, const JavaRef<jobject>& j_sink)
    : j_sink_(jni, j_sink) {}

AudioSinkWrapper::~AudioSinkWrapper() {}

void AudioSinkWrapper::OnData(const void* audio_data,
                    int bits_per_sample,
                    int sample_rate,
                    size_t number_of_channels,
                    size_t number_of_frames) {
  long size = bits_per_sample * number_of_channels * number_of_frames;
  JNIEnv* env = AttachCurrentThreadIfNeeded();
  jbyteArray byte_array = env->NewByteArray(size);
  env->SetByteArrayRegion(byte_array, 0, size, reinterpret_cast<const jbyte*>(audio_data));
  ScopedJavaLocalRef<jbyteArray> jaudio_data = ScopedJavaLocalRef<jbyteArray>(env, byte_array);
  Java_AudioSink_onData(env, j_sink_,
                        jaudio_data,
                        bits_per_sample,
                        sample_rate,
                        number_of_channels,
                        number_of_frames);
}

}  // namespace jni
}  // namespace webrtc

