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


    void connect(const std::string& ip, const uint16_t port)
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

    
    void UploadedPercentage(const std::string& file, const int& prog)
    {
        SInfo << "Percentage uploaded " << prog;
    }


    void cbFailure(const std::string& file, const std::string &reason, const int &code )
    {
        SInfo << "Uploade failure. "  <<  reason;

    }

    void cbSuccess(const std::string& file, const std::string &reason)
    {
        SInfo << "Uploade Suceess. "  << reason;
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


