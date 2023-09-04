#ifndef INET_ADDR_H
#define INET_ADDR_H

#include <arpa/inet.h>
#include <string.h>

#include <string>

class InetAddr {
public:
    InetAddr(std::string ip = "192.168.0.165", int port = 80);
    ~InetAddr();
    std::string ip() const {
        return ip_;
    }
    int port() const {
        return port_;
    }
    int listenFd() const {
        return listenfd_;
    }
private:
    std::string ip_;
    int port_;
    int listenfd_;
    sockaddr_in addr_;
};

#endif