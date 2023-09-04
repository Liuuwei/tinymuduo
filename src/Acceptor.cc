#include "Acceptor.h"
#include "Log.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <string>

Acceptor::Acceptor() : listenFd_(0) {
    
}

Acceptor::~Acceptor() {

}

std::pair<int, std::string> Acceptor::acceptNew() const {
    sockaddr_in peerAddr{};
    socklen_t len = sizeof(peerAddr);
    int fd = 0;
    while ( (fd = accept4(listenFd_, (sockaddr*)&peerAddr, &len, SOCK_NONBLOCK)) == -1) {
        if (errno != EAGAIN || errno != EWOULDBLOCK) {
            return {-1, ""};
        }
    }
    std::string ip(64, '\0');
    inet_ntop(AF_INET, &peerAddr.sin_addr, &*ip.begin(), ip.size());
    return {fd, ip};
}