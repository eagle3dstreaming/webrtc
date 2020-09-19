//
// Created by root on 28/12/19.
//

#ifndef SA_SOCKETIO_H
#define SA_SOCKETIO_H


#include <string>


namespace sa {



  	void connect(const std::string& host, const uint16_t port);

    void  stop( );

    void UploadedPercentage(const std::string& file, const int& prog);

    void cbFailure(const std::string& file, const std::string &reason, const int &code );
    
    void cbSuccess(const std::string& file, const std::string &reason);
    
 

}// end sa



#endif //MEDIAAPP_UPLOAD_H
