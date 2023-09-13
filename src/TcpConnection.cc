#include "TcpConnection.h"
#include "EventLoop.h"
#include "Log.h"

#include <unistd.h>

TcpConnection::TcpConnection(EventLoop* loop, int fd, const std::string& ip) : loop_(loop), channel_(std::make_shared<Channel>(loop, fd)),
                                                                                ip_(ip), maxBytes_(INT_MAX) {

}

TcpConnection::~TcpConnection() {
    Log::Instance()->LOG("%s close connection", ip_.c_str());
    handleClose();
}

void TcpConnection::start() {
    channel_->setIp(ip_);
    channel_->setReadCallback([this] { readCallback(); });
    channel_->setWriteCallback([this] { writeCallback(); });
    channel_->enableRead();
}

void TcpConnection::setReadCallback(const MessageCallback& cb) {
    readCallback_ = cb;
}

void TcpConnection::setWriteCallback(const MessageCallback& cb) {
    writeCallback_ = cb;
}

void TcpConnection::readCallback() {
    loop_->inertToTimeWheel(shared_from_this());
    ssize_t n = inputBuffer_.readFd(channel_->fd());
    if (n == 0) {
        handleClose();
        return;
    }
    if (n == -1) {
        if (errno != EAGAIN || errno != EWOULDBLOCK)
            handleClose();
        return;
    }
    /* 数据量太大认为是异常连接直接关闭 */
    if (inputBuffer_.readAbleBytes() > maxBytes_) {
        handleClose();
        return;
    }
    if (readCallback_) {
        readCallback_(shared_from_this(), &inputBuffer_);
    }
}

void TcpConnection::writeCallback() {
    loop_->inertToTimeWheel(shared_from_this());
    ssize_t sendAble = outputBuffer_.readAbleBytes();
    if (sendAble == 0) {
        channel_->unableWrite();
        return;
    }
    ssize_t n = outputBuffer_.writeFd(channel_->fd());
    if (n == -1) {
        handleClose();
        return;
    }
    if (n == sendAble) {
        if (writeCallback_)
            writeCallback_(shared_from_this(), &outputBuffer_);
        channel_->unableWrite();
    }
}

void TcpConnection::handleClose() {
    channel_->shutdown();
}

int TcpConnection::send(const std::string& msg) {
    loop_->inertToTimeWheel(shared_from_this());
    if (!msg.empty())
        outputBuffer_.append(msg);
    ssize_t sendAble = outputBuffer_.readAbleBytes();
    ssize_t n = outputBuffer_.writeFd(channel_->fd());
    if (n == -1) {
        handleClose();
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            return -1;
        } else {
            return 0;
        }
    }
    if (n == sendAble) {
        channel_->unableWrite();
        channel_->unableRevents(EPOLLOUT);
        if (writeCallback_)
            writeCallback_(shared_from_this(), &outputBuffer_);
    } else {
        channel_->enableWrite();
    }
    return n;
}

void TcpConnection::setIp(const std::string &ip) {
    ip_ = ip;
}
