

#ifndef Signaler_H
#define Signaler_H

#include "socketio/socketioClient.h"
#include "base/thread.h"

using namespace base::sockio;

namespace base {
namespace SdpParse {


    
        class Signaler :public Thread {
        public:
            
            Signaler(const std::string ip, const uint16_t port) ;
            
            ~Signaler();

            void connect(const std::string& host, const uint16_t port);
            
          
            std::string  sfuID;
            
            void run() ;
            
            void shutdown() ;
            
        protected:
            
             std::mutex g_shutdown_mutex;

            // /// PeerManager interface
             
            // void sendCandidate(wrtc::Peer* conn, const std::string& mid, int mlineindex, const std::string& sdp) override;
            // void onAddRemoteStream(wrtc::Peer* conn, webrtc::MediaStreamInterface* stream) override;
            // void onRemoveRemoteStream(wrtc::Peer* conn, webrtc::MediaStreamInterface* stream) override;
            // void onStable(wrtc::Peer* conn) override;
            // void onClosed(wrtc::Peer* conn) override;
            // void onFailure(wrtc::Peer* conn, const std::string& error) override;

        public:
            void sendSDP( const std::string& type, const std::string& sdp,  const std::string & remoteID, const std::string & from);
            void postMessage(const json& m);
            void postAppMessage(const json& m);
                      // void onPeerConnected(std::string& peerID);
            void onPeerMessage(std::string &room , json const& m);
           // void onPeerDiconnected(std::string& peerID);

            
                /// PeerManager interface
            //void sendSDP(wrtc::Peer* conn, const std::string& type, const std::string& sdp) override;
           // void sendCandidate(wrtc::Peer* conn, const std::string& mid, int mlineindex, const std::string& sdp) override;
      
            //void recvSDP(const std::string& token, const json& data);
            void recvCandidate(const std::string& token, const json& data);
            
           // void onPeerConnected(std::string& peerID);
                        
            //void onffer(std::string &room,  std::string& participant, const json &sdp);
           
            //void onPeerDiconnected(std::string &room,  std::string& peerID);
            

        protected:
#if USE_SSL
            //  SocketioSecClient *client;
#else
            SocketioClient *client;
            Socket *socket;

#endif
//            wrtc::MultiplexMediaCapturer _capturer;
//            wrtc::PeerFactoryContext _context;

            //socket* socket{nullptr};

            bool isChannelReady{false};
            bool isInitiator{false};
            bool isStarted{false};
            
            
            std::string m_IP;
            uint16_t m_port;
            
            std::string room;
            
            
            bool shuttingDown;
            

        };


} // namespace SdpParse {

}
#endif
