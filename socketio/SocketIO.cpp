//
// Created by root on 28/12/19.
//

#include "SocketIO.h"
#include "base/logger.h"
#include "base/thread.h"
#include "Signaler.h"
#include <functional>
#include <thread>

using namespace base;
using namespace base::SdpParse;

/*****************************************************************************************/

namespace sa {


    Signaler *thread = nullptr ;


    void connect(const char* ip, const int port, const char** turn_urls,
                 const int no_of_urls,
                 const char* username,
                 const char* credential,
                 bool mandatory_receive_video)
    {
       // sigObj.connect(host, port );
        
        

        Logger::instance().add(new ConsoleChannel("debug", Level::Trace));

        if(!thread)
        {

            SInfo << "connect " << connect  <<  " port " << port ;

            thread = new Signaler(ip, port);
            
            thread->start();

           // thread->fnUpdateProgess = std::bind(&UploadedPercentage, _1, _2);
           // thread->fnSuccess = std::bind(&cbSuccess, _1, _2);
           // thread->fnFailure = std::bind(&cbFailure, _1, _2, _3);

        }
    }





    void cbFailure(const std::string& file, const std::string &reason, const int &code )
    {
        SInfo << " failure. "  <<  reason;

    }

    void cbSuccess(const std::string& file, const std::string &reason)
    {
        SInfo << " Suceess. "  << reason;
    }


 

    void  stop( )
    {
        
        
        if(thread)
	    {
          thread->shutdown();
          thread->join();
          delete thread;
          thread = nullptr;
          SInfo << "sa::exit() ";
        }
      
    }



   

}// end hm


