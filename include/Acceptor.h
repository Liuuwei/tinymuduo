#ifndef ACCPETOR_H
#define ACCPETOR_H

#include <arpa/inet.h>

#include <string>

class Acceptor {
public:
    Acceptor();
    ~Acceptor();
    std::pair<int, std::string> acceptNew() const;
    void setListenFd(int fd) {
        listenFd_ = fd;
    }
private:
    int listenFd_;
};

#endif