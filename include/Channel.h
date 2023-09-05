#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>
#include <sys/epoll.h>
#include <string>
#include <memory>

class EventLoop;

class Channel : public std::enable_shared_from_this<Channel> {
public:
    Channel(EventLoop* loop_, int fd);
    ~Channel();
    int fd() const {
        return fd_;
    }
    int events() const {
        return events_;
    }
    int revents() const {
        return revents_;
    }
    void enableRead();
    void enableWrite();
    void unableRead();
    void unableWrite();
    void unableAll();
    void setReadCallback(std::function<void()> cb);
    void setWriteCallback(std::function<void()> cb);
    void setRevents(int events);
    void unableRevents(int events);
    void executeIO();
    void shutdown();
    void setIp(const std::string& ip);
    std::string ip() const { return ip_; }
    bool operator <(const Channel& other) const {
        return fd_ < other.fd_;
    }
    bool operator ==(const Channel& other) const {
        return fd_ == other.fd_;
    }
private:
    int fd_;
    int events_;
    int revents_;
    EventLoop* loop_;
    std::string ip_;
    std::function<void()> readCallback_ = nullptr;
    std::function<void()> writeCallback_ = nullptr;
    void update();
};

#endif