//
// Created by Liuwei on 2023/8/28.
//
#include "Connector.h"

#include <sys/fcntl.h>

Connector::Connector(std::string ip, int port) : clientFd_(0), ip_(ip), port_(port) {
    clientFd_ = socket(AF_INET, SOCK_STREAM, 0);
}

Connector::~Connector() {

}

int Connector::connect() {
    struct sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_);
    serverAddr.sin_addr.s_addr = inet_addr(ip_.c_str());
    socklen_t len = sizeof(serverAddr);

    int ret = ::connect(clientFd_, (sockaddr*)&serverAddr, len);
    fcntl(clientFd_, SOCK_NONBLOCK);
    return ret;
}