//
// Created by Liuwei on 2023/8/28.
//
#include "TcpClient.h"
#include <assert.h>
#include <netinet/tcp.h>
#include "Log.h"
#include <sys/signal.h>

TcpClient::TcpClient(EventLoop* loop, std::string ip, int port) : loop_(loop), connector_(ip, port),
                                                              threadPoll_(1, loop_), nagle_(true) {
    signal(SIGPIPE, SIG_IGN);

}

TcpClient::~TcpClient() {

}

bool TcpClient::connect() {
    int ret = connector_.connect();
    if (ret == -1) {
        return false;
    }
    fd_ = connector_.fd();
    if (!nagle_) {
        int flag = 1;
        setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    }
    writeConn_ = std::make_shared<TcpConnection>(loop_, fd_, ip_);
    writeConn_->setWriteCallback(writeCallback_);
    readConn_ = std::make_shared<TcpConnection>(threadPoll_.getOneLoop(), fd_, ip_);
    readConn_->setReadCallback(readCallback_);
    if (onGetConnection_) {
        onGetConnection_(writeConn_);
    }
    return true;
}

int TcpClient::send(const std::string& msg) {
    int n = writeConn_->send(msg);
    if (n == -1) {
        Log::Instance()->DEBUG("send -1 exit(0)");
    }
    return n;
}

void TcpClient::setOnConnection(const std::function<void(std::shared_ptr < TcpConnection > )> &cb) {
    onGetConnection_ = cb;
}