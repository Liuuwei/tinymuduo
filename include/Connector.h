//
// Created by Liuwei on 2023/8/28.
//

#ifndef WEBSERVER_CONNECTOR_H
#define WEBSERVER_CONNECTOR_H

#include "InetAddr.h"

class Connector {
public:
    Connector(std::string ip, int port);
    ~Connector();
    int connect();
    int fd() const { return clientFd_; }
private:
    int clientFd_;
    std::string ip_;
    int port_;
};

#endif //WEBSERVER_CONNECTOR_H
