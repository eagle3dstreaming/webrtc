
#include <iostream>
#include <string>

#include "base/logger.h"

#include "Signaler.h"

using std::endl;

namespace base {
namespace SdpParse {

        Signaler::Signaler(const std::string ip, const uint16_t port) : m_IP(ip), m_port(port),shuttingDown(false)
        {
            room = "room1";
           // Logger::instance().add(new ConsoleChannel("debug", Level::Trace));
            Logger::instance().add(new ConsoleChannel("debug", Level::Trace));

        }

        Signaler::~Signaler() {

        }
        
        
        void Signaler::shutdown() {
    
            std::lock_guard<std::mutex> guard(g_shutdown_mutex);
    
            if(!shuttingDown )
            {
                shuttingDown =true;
                
                client->close(0, "terminate");
                
                delete client;
                
            }

        

            STrace << "shutdown over";
        }


        
        void Signaler::run() {

            SInfo << "run";

            Application app;

            connect(m_IP, m_port);

            app.run();
          
            SInfo << "run over";

        }


        void Signaler::postAppMessage(const json& m) {

            LTrace("postAppMessage", cnfg::stringify(m));
            socket->emit("postAppMessage", m);
        }
        ////////////////////////////////////////////////////////////////////

        void Signaler::sendSDP(const std::string& type,  const std::string& sdp, const std::string & remotePeerID, const std::string & from  ) {
            assert(type == "offer" || type == "answer");
            //smpl::Message m;
            json desc;
            desc["type"] = type;
            desc["sdp"] = sdp;

            json m;

            m["type"] = type;
            m["desc"] = desc;
            m["from"] = from;
            m["to"] = remotePeerID;


            postMessage(m);
        }

        void Signaler::postMessage(const json& m) {

            LTrace("postMessage", cnfg::stringify(m));
            socket->emit("message", m);
        }

        
          void Signaler::onPeerMessage(std::string &name , json const& m) {

             
            SInfo <<  "onPeerMessage "  << name;
             
            LTrace(cnfg::stringify(m));
                

            std::string from;
            std::string type;
            std::string room;
            std::string to;
            
            
            if (m.find("to") != m.end()) {
                to = m["to"].get<std::string>();
            }
            

            if (m.find("from") != m.end()) {
                from = m["from"].get<std::string>();
            }
            else
            {
                SError << " On Peer message is missing participant id ";
                return;
            }
            
            if (m.find("type") != m.end()) {
                type = m["type"].get<std::string>();
            }else
            {
                SError << " On Peer message is missing SDP type";
            }
            
            if (m.find("room") != m.end()) {
                room = m["room"].get<std::string>();
            }
            else
            {
                SError << " On Peer message is missing room id ";
                return;
            }
            


            if (std::string("offer") == type) {
                //assert(0 && "offer not supported");

                //std::string remotePeerID = from;
                //onffer(room, from, m["desc"]);
              //  rooms->on_producer_offer( room,  from, m["desc"] );

            } else if (std::string("answer") == type) {
               // recvSDP(from, m["desc"]);
                //rooms->on_consumer_answer( room, from, to, m["desc"] );
            } else if (std::string("candidate") == type) {
                recvCandidate(from, m["candidate"]);
            } else if (std::string("bye") == type) {
               // rooms->onDisconnect( from);
            } 

        }



        void Signaler::recvCandidate(const std::string& token, const json& data) {
            SDebug << "recvCandidate " << token << "  " << data;

        }


        //////////////////////////////////////////////////////////////////////////////////////

        void Signaler::connect(const std::string& host, const uint16_t port) {

   
            client = new SocketioClient(host, port, true);
            client->connect();

            socket = client->io();

            socket->on("connection", Socket::event_listener_aux([ & ](string const& name, json const& data, bool isAck, json & ack_resp) {

                    socket->on("created", Socket::event_listener_aux([&](string const& name, json const& data, bool isAck, json & ack_resp){
                    
                    sfuID = data[1].get<std::string>();   
                    
                    if(data.size() > 2) // for ORTC
                    {
                        SInfo << "Created room " << cnfg::stringify(data[0]) << " - my client ID is " << cnfg::stringify(data[1]);
     
                    }
                    else // webrtc
                    {
                        //rooms->createRoom(name, data, isAck, ack_resp);
                    }

                }));




                socket->on("full", Socket::event_listener_aux([&](string const& name, json const& data, bool isAck, json & ack_resp) {
                    LTrace(cnfg::stringify(data));
                    //LTrace("Room " + room + " is full.")
                        SInfo <<  "Room "  << room  << " is full";
                }));


                socket->on("join", Socket::event_listener_aux([&](string const& name, json const& data, bool isAck, json & ack_resp) {
                    LTrace(cnfg::stringify(data));
                     LTrace("Another peer made a request to join room " + room)
                    LTrace("This peer is the initiator of room " + room + "!")
                    isChannelReady = true;

                }));
                
                socket->on("joined", Socket::event_listener_aux([&](string const& name, json const& data, bool isAck, json & ack_resp) {
                    LTrace(cnfg::stringify(data));
                    LTrace("joined: " + room);
                    isChannelReady = true;
                    isInitiator = true;
                   // maybeStart();

                }));

   /// for webrtc messages
                socket->on("message", Socket::event_listener_aux([&](string const& name, json const& m, bool isAck, json & ack_resp) {
                    //  LTrace(cnfg::stringify(m));
                    // LTrace('SocketioClient received message:', cnfg::stringify(m));

                    onPeerMessage((string &)name, m);
                    // signalingMessageCallback(message);


                }));



                // Leaving rooms and disconnecting from peers.
                socket->on("disconnectClient", Socket::event_listener_aux([&](string const& name, json const& data, bool isAck, json & ack_resp) {
                    std::string participant = data;
                   // rooms->onDisconnect( participant);
                    
                }));


                socket->on("bye", Socket::event_listener_aux([&](string const& name, json const& data, bool isAck, json & ack_resp) {
                    LTrace(cnfg::stringify(data));
                 //  LTrace("Peer leaving room", room);

                }));
                   
                    socket->emit("create or join", room);
            }));

        }


} // namespace SdpParse 
}