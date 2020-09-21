
#include "SocketIO.h"
#include <unistd.h>
#include <iostream>

 const char ip[] = "192.168.0.16";
   
 const int port = 8080;

int main(int argc, char** argv) {

//    sa::stop();
   
    sa::connect(ip, port, nullptr, 2, nullptr,nullptr,true);

    usleep(900000000);
    std::cout << "Signal Client Terminating" << std::endl << std::flush;
    

    // hm::stop();
    // std::cout << "stop done" << std::endl;

    // usleep(900000);

    // std::cout << "second upload start" << std::endl;

    // hm::upload("driver-1234", metadata, file);

    // usleep(9000000);

    // std::cout << "second upload done" << std::endl;

     sa::stop();


    return 0;
}


