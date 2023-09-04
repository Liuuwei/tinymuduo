#include "Channel.h"
#include "EventLoop.h"
#include "Util.h"

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
    loop_->updateInLoop(this);
}

void Channel::setReadCallback(std::function<void()> cb) {
    readCallback = cb;
}
void Channel::setWriteCallback(std::function<void()> cb) {
    writeCallback = cb;
}


void Channel::handleEvent() {
    if (revents_ & EPOLLIN) {
        if (readCallback)
            readCallback();
    }
    if (revents_ & EPOLLOUT) {
        if (writeCallback)
            writeCallback();
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
