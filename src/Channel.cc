#include "Channel.h"
#include "EventLoop.h"

Channel::Channel(EventLoop* loop, int fd) : fd_(fd), events_(0), revents_(0), loop_(loop) {
    
}

Channel::~Channel() {
    ::close(fd_);
}

void Channel::enableRead() {
    events_ |= EPOLLIN;
    update();
}

void Channel::enableWrite() {
    events_ |= EPOLLOUT;
    update();
}

void Channel::unableRead() {
    events_ &= ~EPOLLIN;
    update();
}

void Channel::unableWrite() {
    events_ &= ~EPOLLOUT;
    update();
}

void Channel::setRevents(int events) {
    revents_ |= events;
}

void Channel::unableRevents(int events) {
    revents_ &= ~events;
}

void Channel::update() {
    loop_->updateInLoop(shared_from_this());
}

void Channel::setReadCallback(std::function<void()> cb) {
    readCallback_ = cb;
}
void Channel::setWriteCallback(std::function<void()> cb) {
    writeCallback_ = cb;
}

void Channel::executeIO() {
    if (revents_ & EPOLLIN) {
        if (readCallback_)
            readCallback_();
    }
    if (revents_ & EPOLLOUT) {
        if (writeCallback_)
            writeCallback_();
    }
}

void Channel::shutdown() {
    unableAll();
}

void Channel::unableAll() {
    events_ = 0;
    update();
}

void Channel::setIp(const std::string& ip) {
    ip_ = ip;
}
