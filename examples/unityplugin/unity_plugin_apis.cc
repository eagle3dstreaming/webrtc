/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "examples/unityplugin/unity_plugin_apis.h"

#include <map>
#include <string>

#include "examples/unityplugin/simple_peer_connection.h"
#include "socketio/SocketIO.h"

namespace {
static int g_peer_connection_id = 1;
static std::map<int, rtc::scoped_refptr<SimplePeerConnection>>
    g_peer_connection_map;
}  // namespace

//static bool signalerLoaded= false;
#define MaxClients 5

int CreatePeerConnection(const char* ip, const int port, const char* roomid, const char** turn_urls,
                         const int no_of_urls,
                         const char* username,
                         const char* credential,
                         bool mandatory_receive_video) {
  g_peer_connection_map[g_peer_connection_id] =
      new rtc::RefCountedObject<SimplePeerConnection>(g_peer_connection_id);


    if (!g_peer_connection_map[g_peer_connection_id]->InitializePeerConnection(
            turn_urls, no_of_urls, username, credential, mandatory_receive_video))
        return -1;

    for( int cl =1; cl <= MaxClients; cl++)
    {
        g_peer_connection_map[g_peer_connection_id + cl] =
                new rtc::RefCountedObject<SimplePeerConnection>(g_peer_connection_id + cl);
        if (!g_peer_connection_map[g_peer_connection_id + cl]->InitializePeerConnection(
                turn_urls, no_of_urls, username, credential, mandatory_receive_video))
            return -1;
    }

 // AddStream( 1, false) ;// arvind moved this code to c#
  if (g_peer_connection_id == 1)
  {
      sa::connect(ip, port, roomid, turn_urls, no_of_urls, username, credential, mandatory_receive_video);

  }

  return g_peer_connection_id++;
}

bool ClosePeerConnection(int peer_connection_id) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  g_peer_connection_map[peer_connection_id]->DeletePeerConnection();
  g_peer_connection_map.erase(peer_connection_id);

  for( int cl =1; cl <= MaxClients; cl++)
  {
      g_peer_connection_map[peer_connection_id+cl]->DeletePeerConnection();
      g_peer_connection_map.erase(peer_connection_id+cl);
  }


  g_peer_connection_id =1;

  sa::stop();


  return true;
}

bool AddStream(int peer_connection_id, bool audio_only) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  g_peer_connection_map[peer_connection_id]->AddStreams(audio_only);
  return true;
}

bool AddDataChannel(int peer_connection_id) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  return g_peer_connection_map[peer_connection_id]->CreateDataChannel();
}

bool CreateOffer(int peer_connection_id) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;



    return g_peer_connection_map[peer_connection_id]->CreateOffer(

            [](  const std::string& type, const std::string& sdp)
              {
                  sa::createoffer( type, sdp );
              }

            );
}


bool CreateAnswer(int peer_connection_id) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  return g_peer_connection_map[peer_connection_id]->CreateAnswer(nullptr);
}

 bool CreateAnswer_cb(int peer_connection_id, std::function<void(std::string type, std::string sdp) > fSdp)
{
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  return g_peer_connection_map[peer_connection_id]->CreateAnswer(fSdp);

}



void OnIce(int peer_connection_id, std::function<void( const std::string& candidate, const int sdp_mline_index, const std::string& sdp_mid)> fIce)
{
    if (!g_peer_connection_map.count(peer_connection_id))
        return ;

    return g_peer_connection_map[peer_connection_id]->OnIce(fIce);
}



bool SendDataViaDataChannel(int peer_connection_id, const char* data) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  std::string s(data);
  g_peer_connection_map[peer_connection_id]->SendDataViaDataChannel(s);

  return true;
}

bool SetAudioControl(int peer_connection_id, bool is_mute, bool is_record) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  g_peer_connection_map[peer_connection_id]->SetAudioControl(is_mute,
                                                             is_record);
  return true;
}

bool SetRemoteDescription(int peer_connection_id,
                          const char* type,
                          const char* sdp) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  return g_peer_connection_map[peer_connection_id]->SetRemoteDescription(type,
                                                                         sdp);
}

bool AddIceCandidate(const int peer_connection_id,
                     const char* candidate,
                     const int sdp_mlineindex,
                     const char* sdp_mid) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  return g_peer_connection_map[peer_connection_id]->AddIceCandidate(
      candidate, sdp_mlineindex, sdp_mid);
}

// Register callback functions.
bool RegisterOnLocalI420FrameReady(int peer_connection_id,
                                   I420FRAMEREADY_CALLBACK callback) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  g_peer_connection_map[peer_connection_id]->RegisterOnLocalI420FrameReady(
      callback);
  return true;
}

bool RegisterOnRemoteI420FrameReady(int peer_connection_id,
                                    I420FRAMEREADY_CALLBACK callback) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  g_peer_connection_map[peer_connection_id]->RegisterOnRemoteI420FrameReady(
      callback);

  for( int cl =1; cl <= MaxClients; cl++)
  {
      g_peer_connection_map[peer_connection_id+cl]->RegisterOnRemoteI420FrameReady(
                callback);
  }

  return true;
}

bool RegisterOnLocalDataChannelReady(int peer_connection_id,
                                     LOCALDATACHANNELREADY_CALLBACK callback) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  g_peer_connection_map[peer_connection_id]->RegisterOnLocalDataChannelReady(
      callback);
  return true;
}

bool RegisterOnDataFromDataChannelReady(
    int peer_connection_id,
    DATAFROMEDATECHANNELREADY_CALLBACK callback) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  g_peer_connection_map[peer_connection_id]->RegisterOnDataFromDataChannelReady(
      callback);
  return true;
}

bool RegisterOnFailure(int peer_connection_id, FAILURE_CALLBACK callback) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  g_peer_connection_map[peer_connection_id]->RegisterOnFailure(callback);
  return true;
}

bool RegisterOnAudioBusReady(int peer_connection_id,
                             AUDIOBUSREADY_CALLBACK callback) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  g_peer_connection_map[peer_connection_id]->RegisterOnAudioBusReady(callback);
  return true;
}

// Singnaling channel related functions.
bool RegisterOnLocalSdpReadytoSend(int peer_connection_id,
                                   LOCALSDPREADYTOSEND_CALLBACK callback) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  g_peer_connection_map[peer_connection_id]->RegisterOnLocalSdpReadytoSend(
      callback);
  return true;
}

bool RegisterOnIceCandiateReadytoSend(
    int peer_connection_id,
    ICECANDIDATEREADYTOSEND_CALLBACK callback) {
  if (!g_peer_connection_map.count(peer_connection_id))
    return false;

  g_peer_connection_map[peer_connection_id]->RegisterOnIceCandiateReadytoSend(
      callback);
  return true;
}


void I420_PushFrame(int peer_connection_id, const uint8_t* data_y,
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

    if (!g_peer_connection_map.count(peer_connection_id))
        return;

    g_peer_connection_map[peer_connection_id]->I420_PushFrame(
    data_y,
    data_u,
    data_v,
    data_a,
    stride_y,
    stride_u,
    stride_v,
    stride_a,
    width,
    height);

}

void I420_PushFrameRGBA( int peer_connection_id, const uint8_t*  rgbBuf, int bufLen, const uint8_t*  augBuf,
                                           int augLen, uint32_t width, uint32_t height)
{

  if (!g_peer_connection_map.count(peer_connection_id))
    return;

  g_peer_connection_map[peer_connection_id]->I420_PushFrameRGBA(   rgbBuf,   bufLen,   augBuf,   augLen,   width,   height);

}
