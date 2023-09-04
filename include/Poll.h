#ifndef POLL_H
#define POLL_H

#include "Channel.h"

#include <arpa/inet.h>
#include <sys/epoll.h>

#include <unordered_map>
#include <unordered_set>

class EventLoop;

class Poll {
public:
    Poll();
    ~Poll();
    void poll();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
private:
    int epollFd_;
    EventLoop* loop_;
    std::unordered_map<int, Channel*> channelMaps_;
    std::vector<epoll_event> events_;
    std::vector<Channel*> activeChannels_;
    void update(int operation, Channel* channel) const;
    void fillActiveChannel(int ret);
    void removeActiveChannel(Channel* channel);
};

#endif