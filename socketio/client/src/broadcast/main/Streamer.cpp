
#include "base/filesystem.h"
#include "base/application.h"
#include "base/util.h"
#include "base/idler.h"
#include "base/logger.h"

#include "rtc_base/ssl_adapter.h"

#include "webrtc/signaler.h"

using namespace std;
using namespace base;

/*
// Detect Memory Leaks
#ifdef _DEBUG
#include "MemLeakDetect/MemLeakDetect.h"
#include "MemLeakDetect/MemLeakDetect.cpp"
CMemLeakDetect memLeakDetect;
#endif
 */

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 8080 //443
#define JOIN_ROOM  "foo"        

#include "webrtc/signaler.h"

//std::string sampleDataDir(const std::string& file) {
//    std::string dir;
//   // fs::addnode(dir, base_SOURCE_DIR);
//    fs::addnode(dir, "/");
//    fs::addnode(dir, "var");
//    fs::addnode(dir, "tmp");
//    if (!file.empty())
//        fs::addnode(dir, file);
//    return dir;
//}

int main(int argc, char** argv) {
    Logger::instance().add(new ConsoleChannel("info", Level::Info));


    // Setup WebRTC environment
    rtc::LogMessage::LogToDebug(rtc::LS_ERROR); // LS_VERBOSE, LS_INFO, LS_ERROR
    // rtc::LogMessage::LogTimestamps();
    // rtc::LogMessage::LogThreads();

    rtc::InitializeSSL();


    Application app;

    //std::string sourceFile(sampleDataDir("test.mp3"));

    base::wrtc::Signaler sig;

    //sig.startStreaming("/var/tmp/songs", "", "mp3",  false);
    
    #ifdef __linux__

    sig.startStreaming("/var/tmp/videos", "", "mp4", false);

    #elif _WIN32
        sig.startStreaming("c:/videos", "", "mp4", false);
    #endif   

   
    
   // sig.startStreaming("/var/tmp/videos", "", "264",  false);
    
    //sig.startStreaming("", "/var/tmp/test.mp4", "mp4",  true); // single file play in loop, this feauture migt be broken.  // obsolete apis do not do this
    
   // sig.startStreaming("", "/var/tmp/test.264", "264",  false); // single file play in loop, this feauture migt be broken. // obsolete apis do not do this

    sig.connect(SERVER_HOST, SERVER_PORT, JOIN_ROOM);

    // test._capturer.start();

//    auto rtcthread = rtc::Thread::Current();
//    Idler rtc([rtcthread]() {
//        rtcthread->ProcessMessages(3);
//       // LTrace(" rtcthread->ProcessMessages")
//        base::sleep(1000);
//    });

    LTrace("app.run() run start")
    app.run();
    LTrace("app.run() is over")
    rtc::CleanupSSL();
    Logger::destroy();

    return 0;
}



