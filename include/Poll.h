#ifndef POLL_H
#define POLL_H

#include "Channel.h"

#include <arpa/inet.h>
#include <sys/epoll.h>

#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>

class EventLoop;

class Poll {
public:
    Poll();
    ~Poll();
    void poll();
    void updateChannel(std::shared_ptr<Channel> channel);
    void removeChannel(std::shared_ptr<Channel> channel);
private:
    int epollFd_;
    EventLoop* loop_;
    std::unordered_map<int, std::weak_ptr<Channel>> channelMaps_;
    std::vector<epoll_event> events_;
    std::vector<std::weak_ptr<Channel>> activeChannels_;
    void update(int operation, std::shared_ptr<Channel> channel) const;
    void fillActiveChannel(int ret);
};

#endif