//
// Created by root on 28/12/19.
//

#ifndef SA_SOCKETIO_H
#define SA_SOCKETIO_H


#include <string>


namespace sa {



  	void  connect(const char* signalip, const int signalport, const char** turn_urls,
					   const int no_of_urls,
					   const char* username,
					   const char* credential,
					   bool mandatory_receive_video);

    void  stop( );


	void createoffer( const std::string& type, const std::string& sdp);
    

    
 

}// end sa



#endif //MEDIAAPP_UPLOAD_H
