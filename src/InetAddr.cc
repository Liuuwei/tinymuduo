#include "InetAddr.h"

#include <assert.h>
#include <fcntl.h>

InetAddr::InetAddr(std::string ip, int port) : ip_(ip), port_(port) {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);

    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(listenfd_, O_NONBLOCK);
    int opt = 1;
    setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));
    int ret = bind(listenfd_, (sockaddr*)&addr_, sizeof(addr_));
    assert(ret != -1);
    ret = listen(listenfd_, 1024);
    assert(ret != -1);
}

InetAddr::~InetAddr() {

}