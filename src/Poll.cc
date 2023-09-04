#include "Poll.h"
#include "Log.h"

#include <assert.h>
#include <string.h>
#include <unistd.h>

Poll::Poll() : epollFd_(epoll_create(1)), events_(65536){

}

Poll::~Poll() {

}

void Poll::poll() {
    int ret = epoll_wait(epollFd_, &*events_.begin(), events_.size(), -1);
    if (ret == -1) {
        Log::Instance()->DEBUG("epoll_wait is -1");
    }
    fillActiveChannel(ret);
}

void Poll::fillActiveChannel(int ret) {
    for (int i = 0; i < ret; ++i) {
        int fd = events_[i].data.fd;
        std::string event;
        if (events_[i].events | EPOLLIN) event += "EPOLLIN ";
        if (events_[i].events | EPOLLOUT) event += "EPOLLOUT";
        Log::Instance()->LOG("ip: %s events: %s", channelMaps_[fd]->ip().c_str(), event.c_str());
        channelMaps_[fd]->setRevents(events_[i].events);
        activeChannels_.push_back(channelMaps_[fd]);
    }
    for (auto& activeChannel : activeChannels_) {
        activeChannel->handleEvent();
        activeChannel->unableRevents(activeChannel->revents());
    }
    activeChannels_.clear();
}

void Poll::removeActiveChannel(Channel* channel) {

}

void Poll::updateChannel(Channel* channel) {
    if (!channelMaps_.count(channel->fd())) {
        channelMaps_[channel->fd()] = channel;
        update(EPOLL_CTL_ADD, channel);
    } else {
        update(EPOLL_CTL_MOD, channel);
    }
}

void Poll::removeChannel(Channel* channel) {
    if (channelMaps_.find(channel->fd()) != channelMaps_.end()) {
        channelMaps_.erase(channel->fd());
    }
    update(EPOLL_CTL_DEL, channel);
}

void Poll::update(int operation, Channel* channel) const {
    int fd = channel->fd();
    epoll_event event{};
    event.data.fd = fd;
    event.events = channel->events();
    epoll_ctl(epollFd_, operation, fd, &event);
}