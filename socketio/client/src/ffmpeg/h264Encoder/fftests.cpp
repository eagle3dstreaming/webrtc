//#include "base/define.h"
//#include "base/test.h"
//#include "base/filesystem.h"
//#include "base/application.h"
//#include "base/packetstream.h"
//#include "base/packet.h"
//#include "base/packetqueue.h"
//#include "base/platform.h"

#include "H264_Encoder.h"


using namespace std;
//using namespace base;


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


bool playback_initialized = false;


//#include "H264_Decoder.h"

void frame_callback(AVFrame* frame, AVPacket* pkt, void* user);
void initialize_playback(AVFrame* frame, AVPacket* pkt);

H264_Encoder encoder(NULL, NULL);

H264_Encoder encoder2(NULL, NULL);

int main(int argc, char **argv)
{
   
     encoder2.load(std::string("e:/test9.264"), 25, 1920, 1080);
    //H264_Decoder decoder(frame_callback, NULL);

   // YUV420P_Player player;

    //player_ptr = &player;
//        decoder_ptr = &decoder;

//    if(!decoder.load( "/var/tmp/test1.264", 30.0f)) {
//      ::exit(EXIT_FAILURE);
//    }

    
//     if(!decoder.load( "/workspace/live/mediaServer/test4.264", 30.0f)) {
//      ::exit(EXIT_FAILURE);
//    }
//
//    
//    
//
//
//    while(1)
//    {
//      decoder.readFrame();
//    }
//   
    
#ifdef __linux__ 
    
    std::string fileName("/tmp/test2.264");
    if (argc > 1) {
      fileName = argv[1];
    }

    encoder.load(fileName, 25, 800, 600);
#elif _WIN32
    int w = 800;
    int h = 600;
    std::string fileName("e:/test2.264");
    if (argc > 3) {
      fileName = argv[1];
      w = atoi(argv[2]);
      h = atoi(argv[3]);
    }
    else if (argc > 1) 
    {
      fileName = argv[1];
    
    }
    encoder.load(fileName, 25, w, h);
#else
     encoder.load( std::string("/tmp/test2.264") , 25,  800, 600);
#endif
     
     for (int x = 0; x < 100000; ++x) {
      encoder.encodeFrame();
       encoder2.encodeFrame();
       _sleep(1);
    }


       
     
}
    

void frame_callback(AVFrame* frame, AVPacket* pkt, void* user) {

  if(!playback_initialized) {
    initialize_playback(frame, pkt);
    playback_initialized = true;
  }

   // very old TODONE
    //encoder.encodeFrame(frame->data[0], frame->linesize[0] , frame->data[1], frame->linesize[1], frame->data[2], frame->linesize[2] );
    
//  if(player_ptr) {
//    player_ptr->setYPixels(frame->data[0], frame->linesize[0]);
//    player_ptr->setUPixels(frame->data[1], frame->linesize[1]);
//    player_ptr->setVPixels(frame->data[2], frame->linesize[2]);
//  }
}

void initialize_playback(AVFrame* frame, AVPacket* pkt) {

  if(frame->format != AV_PIX_FMT_YUV420P) {
    printf("This code only support YUV420P data.\n");
    ::exit(EXIT_FAILURE);
  }

   encoder.load( std::string("/tmp/test.264") , 25,  frame->width, frame->height);
//  if(!player_ptr) {
//    printf("player_ptr not found.\n");
//    ::exit(EXIT_FAILURE);
//  }
//
//  if(!player_ptr->setup(frame->width, frame->height)) {
//    printf("Cannot setup the yuv420 player.\n");
//    ::exit(EXIT_FAILURE);
//  }
  
  
  
}
