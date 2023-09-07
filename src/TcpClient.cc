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
    Log::Instance()->close();
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
    loop_->insertNewConnection(writeConn_);

    auto loop = threadPoll_.getOneLoop();
    readConn_ = std::make_shared<TcpConnection>(loop, fd_, ip_);
    readConn_->setReadCallback(readCallback_);
    loop->insertNewConnection(readConn_);

    readConn_->start();
    writeConn_->start();

    if (onGetConnection_) {
        onGetConnection_(writeConn_);
    }
    return true;
}

int TcpClient::send(const std::string& msg) {
    int n = writeConn_->send(msg);
    return n;
}

void TcpClient::setOnConnection(const std::function<void(std::shared_ptr < TcpConnection > )> &cb) {
    onGetConnection_ = cb;
}