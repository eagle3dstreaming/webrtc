/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "examples/unityplugin/simple_peer_connection.h"

#include <utility>

//arvind for test probe
#include <thread>
#include <atomic>
#include <chrono>

//arvind for test probe


#include "absl/memory/memory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/create_peerconnection_factory.h"
#include "media/engine/internal_decoder_factory.h"
#include "media/engine/internal_encoder_factory.h"
#include "media/engine/multiplex_codec_factory.h"
#include "modules/audio_device/include/audio_device.h"
#include "modules/audio_processing/include/audio_processing.h"
#include "modules/video_capture/video_capture_factory.h"
#include "pc/video_track_source.h"
#include "test/vcm_capturer.h"

#if defined(WEBRTC_ANDROID)
#include "examples/unityplugin/class_reference_holder.h"
#include "modules/utility/include/helpers_android.h"
#include "sdk/android/src/jni/android_video_track_source.h"
#include "sdk/android/src/jni/jni_helpers.h"

/// arvind

#include <sdk/android/src/jni/video_encoder_factory_wrapper.h>
#include <sdk/android/src/jni/video_decoder_factory_wrapper.h>
#include <sdk/android/native_api/jni/scoped_java_ref.h>


#endif

// Names used for media stream ids.
const char kAudioLabel[] = "audio_label";
const char kVideoLabel[] = "video_label";
const char kStreamId[] = "stream_id";

namespace {
static int g_peer_count = 0;
static std::unique_ptr<rtc::Thread> g_worker_thread;
static std::unique_ptr<rtc::Thread> g_signaling_thread;

static std::unique_ptr<rtc::Thread> g_network_thread;
static bool fSetReoteDesc =false;
static  std::list< std::unique_ptr <webrtc::IceCandidateInterface> > store_candidate;



////////////////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <third_party/libyuv/include/libyuv.h>

#include "api/scoped_refptr.h"
#include "api/video/i420_buffer.h"
#include "api/video/video_frame_buffer.h"
#include "api/video/video_rotation.h"


    class TestVideoCapturer : public rtc::VideoSourceInterface<webrtc::VideoFrame> {
    public:
        class FramePreprocessor {
        public:
            virtual ~FramePreprocessor() = default;

            virtual webrtc::VideoFrame Preprocess(const webrtc::VideoFrame& frame) = 0;
        };


            ~TestVideoCapturer() = default;

            void OnFrame(const webrtc::VideoFrame& original_frame) {
                int cropped_width = 0;
                int cropped_height = 0;
                int out_width = 0;
                int out_height = 0;

                broadcaster_.OnFrame(original_frame);
                return ;

                webrtc::VideoFrame frame = MaybePreprocess(original_frame);

                if (!video_adapter_.AdaptFrameResolution(
                        frame.width(), frame.height(), frame.timestamp_us() * 1000,
                        &cropped_width, &cropped_height, &out_width, &out_height)) {
                    // Drop frame in order to respect frame rate constraint.
                    return;
                }

                if (out_height != frame.height() || out_width != frame.width()) {
                    // Video adapter has requested a down-scale. Allocate a new buffer and
                    // return scaled version.
                    // For simplicity, only scale here without cropping.
                    rtc::scoped_refptr<webrtc::I420Buffer> scaled_buffer =
                            webrtc::I420Buffer::Create(out_width, out_height);
                    scaled_buffer->ScaleFrom(*frame.video_frame_buffer()->ToI420());

                    webrtc::VideoFrame::Builder new_frame_builder =
                            webrtc::VideoFrame::Builder()
                                    .set_video_frame_buffer(scaled_buffer)
                                    .set_rotation(webrtc::kVideoRotation_0)
                                    .set_timestamp_us(frame.timestamp_us())
                                    .set_id(frame.id());
                    if (frame.has_update_rect()) {
                        webrtc::VideoFrame::UpdateRect new_rect = frame.update_rect().ScaleWithFrame(
                                frame.width(), frame.height(), 0, 0, frame.width(), frame.height(),
                                out_width, out_height);
                        new_frame_builder.set_update_rect(new_rect);
                    }
                    broadcaster_.OnFrame(new_frame_builder.build());

                } else {
                    // No adaptations needed, just return the frame as is.
                    broadcaster_.OnFrame(frame);
                }
            }

            rtc::VideoSinkWants GetSinkWants() {
                return broadcaster_.wants();
            }

            void AddOrUpdateSink(
                    rtc::VideoSinkInterface<webrtc::VideoFrame>* sink,
                    const rtc::VideoSinkWants& wants) {
                broadcaster_.AddOrUpdateSink(sink, wants);
                UpdateVideoAdapter();
            }

            void RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink) {
                broadcaster_.RemoveSink(sink);
                UpdateVideoAdapter();
            }

            void UpdateVideoAdapter() {
                video_adapter_.OnSinkWants(broadcaster_.wants());
            }

            webrtc::VideoFrame MaybePreprocess(const webrtc::VideoFrame& frame) {
                rtc::CritScope crit(&lock_);
                if (preprocessor_ != nullptr) {
                    return preprocessor_->Preprocess(frame);
                } else {
                    return frame;
                }
            }

        rtc::CriticalSection lock_;
        std::unique_ptr<FramePreprocessor> preprocessor_ RTC_GUARDED_BY(lock_);
        rtc::VideoBroadcaster broadcaster_;
        cricket::VideoAdapter video_adapter_;
    };  // namespace webrtc

///////////////////////////////////////////////////////////////////////////////////////////

static rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>
    g_peer_connection_factory;
#if defined(WEBRTC_ANDROID1)
// Android case: the video track does not own the capturer, and it
// relies on the app to dispose the capturer when the peerconnection
// shuts down.
static jobject g_camera = nullptr;
#else

    std::atomic<bool> running(true);

    class TestVcmCapturer : public TestVideoCapturer,
                        public rtc::VideoSinkInterface<webrtc::VideoFrame> {
    public:

        TestVcmCapturer() :  renderThread(nullptr){}

        bool Init(size_t width,
                               size_t height,
                               size_t target_fps,
                               size_t capture_device_index) {
            //std::unique_ptr<VideoCaptureModule::DeviceInfo> device_info(
              //      VideoCaptureFactory::CreateDeviceInfo());
           // renderThread = new std::thread(&TestVcmCapturer::updateFunc, this);
            return true;
        }

        static TestVcmCapturer* Create(size_t width,
                                         size_t height,
                                         size_t target_fps,
                                         size_t capture_device_index) {
            std::unique_ptr<TestVcmCapturer> vcm_capturer(new TestVcmCapturer());
            if (!vcm_capturer->Init(width, height, target_fps, capture_device_index)) {
                RTC_LOG(LS_WARNING) << "Failed to create VcmCapturer(w = " << width
                                    << ", h = " << height << ", fps = " << target_fps
                                    << ")";
                return nullptr;
            }
            return vcm_capturer.release();
        }

        void Destroy() {
           // if (!vcm_)
             //   return;

           // vcm_->StopCapture();
            //vcm_->DeRegisterCaptureDataCallback();
            // Release reference to VCM.
           // vcm_ = nullptr;
        }

        void FillFrameBuffer(rtc::scoped_refptr<webrtc::I420Buffer> frame) {
            libyuv::I420Rect(frame->MutableDataY(), frame->StrideY(),
                                   frame->MutableDataU(), frame->StrideU(),
                                   frame->MutableDataV(), frame->StrideV(), 0, 0,
                                   frame->width(), frame->height(), 12,
                                   23, 34);
        }
        void static updateFunc(rtc::VideoSinkInterface<webrtc::VideoFrame>* dataCallback)
        {
            uint16_t myid= 0;

            while(running)
            {
                //std::lock_guard<std::mutex> locker(mutex);

                rtc::scoped_refptr<webrtc::I420Buffer> scaled_buffer =
                        webrtc::I420Buffer::Create(640, 480);


                libyuv::I420Rect(scaled_buffer->MutableDataY(), scaled_buffer->StrideY(),
                                 scaled_buffer->MutableDataU(), scaled_buffer->StrideU(),
                                 scaled_buffer->MutableDataV(), scaled_buffer->StrideV(), 0, 0,
                                 scaled_buffer->width(), scaled_buffer->height(), 12,
                                 640, 480);


               // FillFrameBuffer(scaled_buffer);

               // scaled_buffer->ScaleFrom(*frame.video_frame_buffer()->ToI420());

                webrtc::VideoFrame::Builder new_frame_builder =
                        webrtc::VideoFrame::Builder()
                                .set_video_frame_buffer(scaled_buffer)
                                .set_rotation(webrtc::kVideoRotation_0)
                                .set_timestamp_us(rtc::TimeMicros())
                                .set_id(++myid);


                dataCallback->OnFrame(  new_frame_builder.build());

                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }

        virtual ~TestVcmCapturer() {
            running=false;
            if(renderThread) {
                renderThread->join();
                delete renderThread;
            }
            Destroy();
        }

        void OnFrame(const webrtc::VideoFrame& frame)
        {
            TestVideoCapturer::OnFrame(frame);
        }


     //   rtc::scoped_refptr<webrtc::VideoCaptureModule> vcm_;

       // static
        //std::mutex    mutex;
        std::thread   *renderThread;


        webrtc::VideoCaptureCapability capability_;
    };

   // std::atomic<bool> TestVcmCapturer::running=true;

class CapturerTrackSource : public webrtc::VideoTrackSource, public rtc::VideoSinkInterface<webrtc::VideoFrame>  {
 public:
  static rtc::scoped_refptr<CapturerTrackSource> Create() {
    const size_t kWidth = 640;
    const size_t kHeight = 480;
    const size_t kFps = 30;
    const size_t kDeviceIndex = 0;
    std::unique_ptr< TestVcmCapturer> capturer = absl::WrapUnique(
            TestVcmCapturer::Create(kWidth, kHeight, kFps, kDeviceIndex));
    if (!capturer) {
      return nullptr;
    }
    return new rtc::RefCountedObject<CapturerTrackSource>(std::move(capturer));
  }

    virtual void OnFrame(const webrtc::VideoFrame& frame) override
    {
        capturer_->OnFrame( frame  );
    }
 protected:
  explicit CapturerTrackSource(
      std::unique_ptr<TestVcmCapturer> capturer)
      : VideoTrackSource(/*remote=*/false), capturer_(std::move(capturer)) {}

 private:
  rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
    return capturer_.get();
  }
  std::unique_ptr<TestVcmCapturer> capturer_;
};

#endif

static  rtc::scoped_refptr<CapturerTrackSource> video_device;

////////////////////////////////////////////////////


webrtc::VideoDecoderFactory* CreateVideoDecoderFactory( JNIEnv* jni,  const  webrtc::JavaRef<jobject>& j_decoder_factory) {
    return   new  webrtc::jni::VideoDecoderFactoryWrapper(jni, j_decoder_factory);
}

webrtc::VideoEncoderFactory* CreateVideoEncoderFactory(  JNIEnv* jni, const webrtc::JavaRef<jobject>& j_encoder_factory)
{
    return  new  webrtc::jni::VideoEncoderFactoryWrapper(jni, j_encoder_factory);
}



///////////////////////////////////////////////////


std::string GetEnvVarOrDefault(const char* env_var_name,
                               const char* default_value) {
  std::string value;
  const char* env_var = getenv(env_var_name);
  if (env_var)
    value = env_var;

  if (value.empty())
    value = default_value;

  return value;
}

std::string GetPeerConnectionString() {
  return GetEnvVarOrDefault("WEBRTC_CONNECT", "stun:stun.l.google.com:19302");
}

class DummySetSessionDescriptionObserver
    : public webrtc::SetSessionDescriptionObserver {
 public:
  static DummySetSessionDescriptionObserver* Create() {
    return new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
  }
  virtual void OnSuccess() { RTC_LOG(INFO) << __FUNCTION__; }
  virtual void OnFailure(webrtc::RTCError error) {
    RTC_LOG(INFO) << __FUNCTION__ << " " << ToString(error.type()) << ": "
                  << error.message();
  }

 protected:
  DummySetSessionDescriptionObserver() {}
  ~DummySetSessionDescriptionObserver() {}
};

}  // namespace

bool SimplePeerConnection::InitializePeerConnection(const char** turn_urls,
                                                    const int no_of_urls,
                                                    const char* username,
                                                    const char* credential,
                                                    bool is_receiver) {
  RTC_DCHECK(peer_connection_.get() == nullptr);

  if (g_peer_connection_factory == nullptr) {
       rtc::InitRandom(rtc::Time());
       rtc::ThreadManager::Instance()->WrapCurrentThread();
       g_network_thread = rtc::Thread::CreateWithSocketServer();
       g_network_thread->SetName("network_thread", nullptr);
       RTC_CHECK(g_network_thread->Start()) << "Failed to start thread";

      g_worker_thread = rtc::Thread::Create();
      g_worker_thread->SetName("worker_thread", nullptr);
      RTC_CHECK(g_worker_thread->Start()) << "Failed to start thread";


      g_signaling_thread = rtc::Thread::Create();
      g_signaling_thread->Start();

      JNIEnv* env = webrtc::jni::GetEnv();
      jclass pc_factory_class =
                  unity_plugin::FindClass(env, "org/webrtc/UnityUtility");
      jmethodID ME = webrtc::GetStaticMethodID(
                  env, pc_factory_class, "MultiplexH264Encoder", "()Lorg/webrtc/VideoEncoderFactory;");
      jobject ME_obj = env->CallStaticObjectMethod( pc_factory_class, ME);



      jmethodID MD = webrtc::GetStaticMethodID(
                env, pc_factory_class, "MultiplexH264Decoder", "()Lorg/webrtc/VideoDecoderFactory;");
      jobject MD_obj = env->CallStaticObjectMethod( pc_factory_class, MD);



      CHECK_EXCEPTION(env);
          RTC_DCHECK(ME_obj != nullptr)
              << "Cannot get VideoEncoderFactory.";


       g_peer_connection_factory = webrtc::CreatePeerConnectionFactory(
               g_network_thread.get(), g_worker_thread.get(), g_signaling_thread.get(),
            nullptr, webrtc::CreateBuiltinAudioEncoderFactory(),
            webrtc::CreateBuiltinAudioDecoderFactory(),

            std::unique_ptr<webrtc::VideoEncoderFactory>(
                    new webrtc::MultiplexEncoderFactory(
                            absl::WrapUnique(CreateVideoEncoderFactory(env,   ( const webrtc::JavaRef<jobject>&) ME_obj )), false ))

                    ,

            std::unique_ptr<webrtc::VideoDecoderFactory>(
                    new webrtc::MultiplexDecoderFactory(
                            absl::WrapUnique(CreateVideoDecoderFactory(env, ( const webrtc::JavaRef<jobject>&) MD_obj)), false ))

                    ,
            nullptr, nullptr);
        /*
        g_peer_connection_factory = webrtc::CreatePeerConnectionFactory(
                network_thread_.get(), g_worker_thread.get(), g_signaling_thread.get(),
            nullptr, webrtc::CreateBuiltinAudioEncoderFactory(),
            webrtc::CreateBuiltinAudioDecoderFactory(),
            std::unique_ptr<webrtc::VideoEncoderFactory>(
                new webrtc::MultiplexEncoderFactory(
                    std::make_unique<webrtc::InternalEncoderFactory>())),
            std::unique_ptr<webrtc::VideoDecoderFactory>(
                new webrtc::MultiplexDecoderFactory(
                    std::make_unique<webrtc::InternalDecoderFactory>())),
            nullptr, nullptr);
         */
  }
  if (!g_peer_connection_factory.get()) {
    DeletePeerConnection();
    return false;
  }

  g_peer_count++;
  if (!CreatePeerConnection(turn_urls, no_of_urls, username, credential)) {
    DeletePeerConnection();
    return false;
  }

  mandatory_receive_ = is_receiver;
  return peer_connection_.get() != nullptr;
}

bool SimplePeerConnection::CreatePeerConnection(const char** turn_urls,
                                                const int no_of_urls,
                                                const char* username,
                                                const char* credential) {
  RTC_DCHECK(g_peer_connection_factory.get() != nullptr);
  RTC_DCHECK(peer_connection_.get() == nullptr);

  local_video_observer_.reset(new VideoObserver());
  remote_video_observer_.reset(new VideoObserver());

  // Add the turn server.
  if (turn_urls != nullptr) {
    if (no_of_urls > 0) {
      webrtc::PeerConnectionInterface::IceServer turn_server;
      for (int i = 0; i < no_of_urls; i++) {
        std::string url(turn_urls[i]);
        if (url.length() > 0)
          turn_server.urls.push_back(turn_urls[i]);
      }

      std::string user_name(username);
      if (user_name.length() > 0)
        turn_server.username = username;

      std::string password(credential);
      if (password.length() > 0)
        turn_server.password = credential;

      config_.servers.push_back(turn_server);
    }
  }

  // Add the stun server.
  webrtc::PeerConnectionInterface::IceServer stun_server;
  stun_server.uri = GetPeerConnectionString();
  config_.servers.push_back(stun_server);
  config_.enable_rtp_data_channel = true;
  config_.enable_dtls_srtp = true;

  peer_connection_ = g_peer_connection_factory->CreatePeerConnection(
      config_, nullptr, nullptr, this);

  return peer_connection_.get() != nullptr;
}

void SimplePeerConnection::DeletePeerConnection() {
  g_peer_count--;

#if defined(WEBRTC_ANDROID1)
  if (g_camera) {
    JNIEnv* env = webrtc::jni::GetEnv();
    jclass pc_factory_class =
        unity_plugin::FindClass(env, "org/webrtc/UnityUtility");
    jmethodID stop_camera_method = webrtc::GetStaticMethodID(
        env, pc_factory_class, "StopCamera", "(Lorg/webrtc/VideoCapturer;)V");

    env->CallStaticVoidMethod(pc_factory_class, stop_camera_method, g_camera);
    CHECK_EXCEPTION(env);

    g_camera = nullptr;
  }
#endif

  CloseDataChannel();
  peer_connection_ = nullptr;
  active_streams_.clear();

  if (g_peer_count == 0) {
    g_peer_connection_factory = nullptr;
    g_signaling_thread.reset();
    g_worker_thread.reset();
  }
}

bool SimplePeerConnection::CreateOffer() {
  if (!peer_connection_.get())
    return false;

  webrtc::PeerConnectionInterface::RTCOfferAnswerOptions options;
  if (mandatory_receive_) {
    options.offer_to_receive_audio = true;
    options.offer_to_receive_video = true;
  }
  peer_connection_->CreateOffer(this, options);
  return true;
}

bool SimplePeerConnection::CreateAnswer() {
  if (!peer_connection_.get())
    return false;

  webrtc::PeerConnectionInterface::RTCOfferAnswerOptions options;
  if (mandatory_receive_) {
    options.offer_to_receive_audio = true;
    options.offer_to_receive_video = true;
  }
  peer_connection_->CreateAnswer(this, options);
  return true;
}

void SimplePeerConnection::OnSuccess(
    webrtc::SessionDescriptionInterface* desc) {
  peer_connection_->SetLocalDescription(
      DummySetSessionDescriptionObserver::Create(), desc);

  std::string sdp;
  desc->ToString(&sdp);

  if (OnLocalSdpReady)
    OnLocalSdpReady(desc->type().c_str(), sdp.c_str());
}

void SimplePeerConnection::OnFailure(webrtc::RTCError error) {
  RTC_LOG(LERROR) << ToString(error.type()) << ": " << error.message();

  // TODO(hta): include error.type in the message
  if (OnFailureMessage)
    OnFailureMessage(error.message());
}

void SimplePeerConnection::OnIceCandidate(
    const webrtc::IceCandidateInterface* candidate) {
  RTC_LOG(INFO) << __FUNCTION__ << " " << candidate->sdp_mline_index();

  std::string sdp;
  if (!candidate->ToString(&sdp)) {
    RTC_LOG(LS_ERROR) << "Failed to serialize candidate";
    return;
  }

  if (OnIceCandiateReady)
    OnIceCandiateReady(sdp.c_str(), candidate->sdp_mline_index(),
                       candidate->sdp_mid().c_str());
}

void SimplePeerConnection::RegisterOnLocalI420FrameReady(
    I420FRAMEREADY_CALLBACK callback) {
  if (local_video_observer_)
    local_video_observer_->SetVideoCallback(callback);
}

void SimplePeerConnection::RegisterOnRemoteI420FrameReady(
    I420FRAMEREADY_CALLBACK callback) {
  if (remote_video_observer_)
    remote_video_observer_->SetVideoCallback(callback);
}

void SimplePeerConnection::RegisterOnLocalDataChannelReady(
    LOCALDATACHANNELREADY_CALLBACK callback) {
  OnLocalDataChannelReady = callback;
}

void SimplePeerConnection::RegisterOnDataFromDataChannelReady(
    DATAFROMEDATECHANNELREADY_CALLBACK callback) {
  OnDataFromDataChannelReady = callback;
}

void SimplePeerConnection::RegisterOnFailure(FAILURE_CALLBACK callback) {
  OnFailureMessage = callback;
}

void SimplePeerConnection::RegisterOnAudioBusReady(
    AUDIOBUSREADY_CALLBACK callback) {
  OnAudioReady = callback;
}

void SimplePeerConnection::RegisterOnLocalSdpReadytoSend(
    LOCALSDPREADYTOSEND_CALLBACK callback) {
  OnLocalSdpReady = callback;
}

void SimplePeerConnection::RegisterOnIceCandiateReadytoSend(
    ICECANDIDATEREADYTOSEND_CALLBACK callback) {
  OnIceCandiateReady = callback;
}

bool SimplePeerConnection::SetRemoteDescription(const char* type,
                                                const char* sdp) {
  if (!peer_connection_)
    return false;

  std::string remote_desc(sdp);
  std::string sdp_type(type);
  webrtc::SdpParseError error;
  webrtc::SessionDescriptionInterface* session_description(
      webrtc::CreateSessionDescription(sdp_type, remote_desc, &error));
  if (!session_description) {
    RTC_LOG(WARNING) << "Can't parse received session description message. "
                        "SdpParseError was: "
                     << error.description;
    return false;
  }
  RTC_LOG(INFO) << " Received session description :" << remote_desc;
  peer_connection_->SetRemoteDescription(
      DummySetSessionDescriptionObserver::Create(), session_description);

  fSetReoteDesc = true;

    for ( std::list< std::unique_ptr <webrtc::IceCandidateInterface> > ::iterator it = store_candidate.begin();
                  it != store_candidate.end(); ++it) {

        if (!peer_connection_->AddIceCandidate(it->get())) {
            RTC_LOG(WARNING) << "Failed to apply the received candidate";
         }
    }

  return true;
}

bool SimplePeerConnection::AddIceCandidate(const char* candidate,
                                           const int sdp_mlineindex,
                                           const char* sdp_mid) {
    //arvind
    if (!peer_connection_)
        return false;

    webrtc::SdpParseError error;
    std::unique_ptr<webrtc::IceCandidateInterface> ice_candidate(
            webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, candidate, &error));
    if (!ice_candidate.get()) {
        RTC_LOG(WARNING) << "Can't parse received candidate message. "
                            "SdpParseError was: "
                         << error.description;
        return true;
    }

    // arvind
    if(!fSetReoteDesc)
    {
        store_candidate.push_back(std::move(ice_candidate));
        return true;
    }


    if (!peer_connection_->AddIceCandidate(ice_candidate.get())) {
        RTC_LOG(WARNING) << "Failed to apply the received candidate";
        return false;
    }
    RTC_LOG(INFO) << " Received candidate :" << candidate;
      return true;
}

void SimplePeerConnection::SetAudioControl(bool is_mute, bool is_record) {
  is_mute_audio_ = is_mute;
  is_record_audio_ = is_record;

  SetAudioControl();
}

void SimplePeerConnection::SetAudioControl() {
  if (!remote_stream_)
    return;
  webrtc::AudioTrackVector tracks = remote_stream_->GetAudioTracks();
  if (tracks.empty())
    return;

  webrtc::AudioTrackInterface* audio_track = tracks[0];
  std::string id = audio_track->id();
  if (is_record_audio_)
    audio_track->AddSink(this);
  else
    audio_track->RemoveSink(this);

  for (auto& track : tracks) {
    if (is_mute_audio_)
      track->set_enabled(false);
    else
      track->set_enabled(true);
  }
}

void SimplePeerConnection::OnAddStream(
    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
  RTC_LOG(INFO) << __FUNCTION__ << " " << stream->id();
  remote_stream_ = stream;
  if (remote_video_observer_ && !remote_stream_->GetVideoTracks().empty()) {
    remote_stream_->GetVideoTracks()[0]->AddOrUpdateSink(
        remote_video_observer_.get(), rtc::VideoSinkWants());
  }
  SetAudioControl();
}

void SimplePeerConnection::AddStreams(bool audio_only) {
  if (active_streams_.find(kStreamId) != active_streams_.end())
    return;  // Already added.

  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
      g_peer_connection_factory->CreateLocalMediaStream(kStreamId);

  rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
      g_peer_connection_factory->CreateAudioTrack(
          kAudioLabel, g_peer_connection_factory->CreateAudioSource(
                           cricket::AudioOptions())));
  std::string id = audio_track->id();
  stream->AddTrack(audio_track);

  if (!audio_only) {
#if defined(WEBRTC_ANDROID1)
    JNIEnv* env = webrtc::jni::GetEnv();
    jclass pc_factory_class =
        unity_plugin::FindClass(env, "org/webrtc/UnityUtility");
    jmethodID load_texture_helper_method = webrtc::GetStaticMethodID(
        env, pc_factory_class, "LoadSurfaceTextureHelper",
        "()Lorg/webrtc/SurfaceTextureHelper;");
    jobject texture_helper = env->CallStaticObjectMethod(
        pc_factory_class, load_texture_helper_method);
    CHECK_EXCEPTION(env);
    RTC_DCHECK(texture_helper != nullptr)
        << "Cannot get the Surface Texture Helper.";

    rtc::scoped_refptr<webrtc::jni::AndroidVideoTrackSource> source(
        new rtc::RefCountedObject<webrtc::jni::AndroidVideoTrackSource>(
            g_signaling_thread.get(), env, /* is_screencast= */ false,
            /* align_timestamps= */ true));

    // link with VideoCapturer (Camera);
    jmethodID link_camera_method = webrtc::GetStaticMethodID(
        env, pc_factory_class, "LinkCamera",
        "(JLorg/webrtc/SurfaceTextureHelper;)Lorg/webrtc/VideoCapturer;");
    jobject camera_tmp =
        env->CallStaticObjectMethod(pc_factory_class, link_camera_method,
                                    (jlong)source.get(), texture_helper);
    CHECK_EXCEPTION(env);
    g_camera = (jobject)env->NewGlobalRef(camera_tmp);

    rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(
        g_peer_connection_factory->CreateVideoTrack(kVideoLabel,
                                                    source.release()));
    stream->AddTrack(video_track);
#else
    //rtc::scoped_refptr<CapturerTrackSource> video_device =
    //    CapturerTrackSource::Create();

    video_device = CapturerTrackSource::Create();

    if (video_device) {
      rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(
          g_peer_connection_factory->CreateVideoTrack(kVideoLabel,
                                                      video_device));
        ///////webrtc::VideoFrame fm();
       /* rtc::scoped_refptr<webrtc::I420Buffer> scaled_buffer =
                webrtc::I420Buffer::Create(640, 480);

        libyuv::I420Rect(scaled_buffer->MutableDataY(), scaled_buffer->StrideY(),
                         scaled_buffer->MutableDataU(), scaled_buffer->StrideU(),
                         scaled_buffer->MutableDataV(), scaled_buffer->StrideV(), 0, 0,
                         scaled_buffer->width(), scaled_buffer->height(), 12,
                         640, 480);


        // FillFrameBuffer(scaled_buffer);

        // scaled_buffer->ScaleFrom(*frame.video_frame_buffer()->ToI420());

        webrtc::VideoFrame::Builder new_frame_builder =
                webrtc::VideoFrame::Builder()
                        .set_video_frame_buffer(scaled_buffer)
                        .set_rotation(webrtc::kVideoRotation_0)
                        .set_timestamp_us(rtc::TimeMicros())
                        .set_id(3);


        //dataCallback->OnFrame(  new_frame_builder.build());
        // TestVideoCapturer::OnFrame(new_frame_builder.build());
      video_device->OnFrame(new_frame_builder.build() );
*/
      stream->AddTrack(video_track);
    }
#endif
    if (local_video_observer_ && !stream->GetVideoTracks().empty()) {
      stream->GetVideoTracks()[0]->AddOrUpdateSink(local_video_observer_.get(),
                                                   rtc::VideoSinkWants());
    }
  }

  if (!peer_connection_->AddStream(stream)) {
    RTC_LOG(LS_ERROR) << "Adding stream to PeerConnection failed";
  }

  typedef std::pair<std::string,
                    rtc::scoped_refptr<webrtc::MediaStreamInterface>>
      MediaStreamPair;
  active_streams_.insert(MediaStreamPair(stream->id(), stream));
}

bool SimplePeerConnection::CreateDataChannel() {
  struct webrtc::DataChannelInit init;
  init.ordered = true;
  init.reliable = true;
  data_channel_ = peer_connection_->CreateDataChannel("Hello", &init);
  if (data_channel_.get()) {
    data_channel_->RegisterObserver(this);
    RTC_LOG(LS_INFO) << "Succeeds to create data channel";
    return true;
  } else {
    RTC_LOG(LS_INFO) << "Fails to create data channel";
    return false;
  }
}

void SimplePeerConnection::CloseDataChannel() {
  if (data_channel_.get()) {
    data_channel_->UnregisterObserver();
    data_channel_->Close();
  }
  data_channel_ = nullptr;
}

bool SimplePeerConnection::SendDataViaDataChannel(const std::string& data) {
  if (!data_channel_.get()) {
    RTC_LOG(LS_INFO) << "Data channel is not established";
    return false;
  }
  webrtc::DataBuffer buffer(data);
  data_channel_->Send(buffer);
  return true;
}

// Peerconnection observer
void SimplePeerConnection::OnDataChannel(
    rtc::scoped_refptr<webrtc::DataChannelInterface> channel) {
  channel->RegisterObserver(this);
}

void SimplePeerConnection::OnStateChange() {
  if (data_channel_) {
    webrtc::DataChannelInterface::DataState state = data_channel_->state();
    if (state == webrtc::DataChannelInterface::kOpen) {
      if (OnLocalDataChannelReady)
        OnLocalDataChannelReady();
      RTC_LOG(LS_INFO) << "Data channel is open";
    }
  }
}

//  A data buffer was successfully received.
void SimplePeerConnection::OnMessage(const webrtc::DataBuffer& buffer) {
  size_t size = buffer.data.size();
  char* msg = new char[size + 1];
  memcpy(msg, buffer.data.data(), size);
  msg[size] = 0;
  if (OnDataFromDataChannelReady)
    OnDataFromDataChannelReady(msg);
  delete[] msg;
}

// AudioTrackSinkInterface implementation.
void SimplePeerConnection::OnData(const void* audio_data,
                                  int bits_per_sample,
                                  int sample_rate,
                                  size_t number_of_channels,
                                  size_t number_of_frames) {
  if (OnAudioReady)
    OnAudioReady(audio_data, bits_per_sample, sample_rate,
                 static_cast<int>(number_of_channels),
                 static_cast<int>(number_of_frames));
}

std::vector<uint32_t> SimplePeerConnection::GetRemoteAudioTrackSsrcs() {
  std::vector<rtc::scoped_refptr<webrtc::RtpReceiverInterface>> receivers =
      peer_connection_->GetReceivers();

  std::vector<uint32_t> ssrcs;
  for (const auto& receiver : receivers) {
    if (receiver->media_type() != cricket::MEDIA_TYPE_AUDIO)
      continue;

    std::vector<webrtc::RtpEncodingParameters> params =
        receiver->GetParameters().encodings;

    for (const auto& param : params) {
      uint32_t ssrc = param.ssrc.value_or(0);
      if (ssrc > 0)
        ssrcs.push_back(ssrc);
    }
  }

  return ssrcs;
}


void SimplePeerConnection::I420_PushFrame(const uint8_t* data_y,
                    const uint8_t* data_u,
                    const uint8_t* data_v,
                    const uint8_t* data_a,
                    int stride_y,
                    int stride_u,
                    int stride_v,
                    int stride_a,
                    uint32_t width,
                    uint32_t height)
{

    static uint16_t myid= 0;
/*
    rtc::scoped_refptr<webrtc::I420Buffer> scaled_buffer =
            webrtc::I420Buffer::Create(640, 480);

    libyuv::I420Rect(scaled_buffer->MutableDataY(), scaled_buffer->StrideY(),
                     scaled_buffer->MutableDataU(), scaled_buffer->StrideU(),
                     scaled_buffer->MutableDataV(), scaled_buffer->StrideV(), 0, 0,
                     scaled_buffer->width(), scaled_buffer->height(), 12,
                     640, 480);


    // FillFrameBuffer(scaled_buffer);

    // scaled_buffer->ScaleFrom(*frame.video_frame_buffer()->ToI420());

    webrtc::VideoFrame::Builder new_frame_builder =
            webrtc::VideoFrame::Builder()
                    .set_video_frame_buffer(scaled_buffer)
                    .set_rotation(webrtc::kVideoRotation_0)
                    .set_timestamp_us(rtc::TimeMicros())
                    .set_id(++myid);


    //dataCallback->OnFrame(  new_frame_builder.build());
    video_device->OnFrame(new_frame_builder.build() );
    */
    rtc::scoped_refptr<webrtc::I420Buffer> scaled_buffer =
            webrtc::I420Buffer::Create(width, height, stride_y, stride_u, stride_v);


    // Arvind infuture use ConvertToI420
    //scaled_buffer->Copy( width, height, data_y, stride_y, data_u,
     //                    stride_u, data_v, stride_v);

     int ret = libyuv::I420Copy(data_y, stride_y, data_u, stride_u, data_v,
                                     stride_v, scaled_buffer->MutableDataY(),
                                 scaled_buffer->StrideY(), scaled_buffer->MutableDataU(),
                                 scaled_buffer->StrideU(), scaled_buffer->MutableDataV(),
                                 scaled_buffer->StrideV(), width, height);

    if (ret < 0) {
        RTC_LOG(LS_ERROR) << "RGBAToI420 error";

        return;  // Out of memory runtime error.
    }

    webrtc::VideoFrame::Builder new_frame_builder =
            webrtc::VideoFrame::Builder()
                    .set_video_frame_buffer(scaled_buffer)
                    .set_rotation(webrtc::kVideoRotation_0)
                    .set_timestamp_us(rtc::TimeMicros())
                    .set_id(++myid);


    //dataCallback->OnFrame(  new_frame_builder.build());
    video_device->OnFrame(new_frame_builder.build() );
}

void SimplePeerConnection::I420_PushFrameRGBA( const uint8_t*  rgbBuf, int bufLen, const uint8_t*  augBuf,
                                               int augLen, uint32_t width, uint32_t height) {

//    libyuv::RGBAToI420(scaled_buffer->MutableDataY(), scaled_buffer->StrideY(),
//                     scaled_buffer->MutableDataU(), scaled_buffer->StrideU(),
//                     scaled_buffer->MutableDataV(), scaled_buffer->StrideV(), 0, 0,
//                     scaled_buffer->width(), scaled_buffer->height(), 12,
//                     640, 480);

    static uint16_t myid = 0;

    int y_size = width * height;
    int uv_size = ((width + 1) / 2) * ((height + 1) / 2);
    uint8_t *rotate_buffer = (uint8_t *) malloc(y_size + uv_size * 2); /* NOLINT */
    if (!rotate_buffer) {
        RTC_LOG(LS_ERROR) << "Out of memory runtime error";
        return;  // Out of memory runtime error.
    }
    uint8_t *dst_y = rotate_buffer;
    uint8_t *dst_u = dst_y + y_size;
    uint8_t *dst_v = dst_u + uv_size;
    int dst_stride_y = width;
    int dst_stride_u = ((width + 1) / 2);
    int dst_stride_v = dst_stride_u;


    int r = libyuv::RGBAToI420(rgbBuf, width * 4, dst_y, dst_stride_y, dst_u,
                               dst_stride_u, dst_v, dst_stride_v, width,
                               height);

    if (r < 0) {
        RTC_LOG(LS_ERROR) << "RGBAToI420 error";
        free(rotate_buffer);
        return;  // Out of memory runtime error.
    }

    rtc::scoped_refptr<webrtc::I420Buffer> scaled_buffer =
            webrtc::I420Buffer::Create(width, height);


   // Arvind infuture use ConvertToI420
    scaled_buffer->Copy( width, height, dst_y, dst_stride_y, dst_u,
                        dst_stride_u, dst_v,dst_stride_v);


    webrtc::VideoFrame::Builder new_frame_builder =
            webrtc::VideoFrame::Builder()
                    .set_video_frame_buffer(scaled_buffer)
                    .set_rotation(webrtc::kVideoRotation_0)
                    .set_timestamp_us(rtc::TimeMicros())
                    .set_id(++myid);


    //dataCallback->OnFrame(  new_frame_builder.build());
    video_device->OnFrame(new_frame_builder.build() );

    free(rotate_buffer);

}