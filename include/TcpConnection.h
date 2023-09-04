#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "Channel.h"
#include "Buffer.h"

#include <memory>

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void(TcpConnectionPtr, Buffer*)> MessageCallback;
public:
    TcpConnection(EventLoop* loop, int fd, const std::string& ip);
    ~TcpConnection();
    void start();
    void setReadCallback(const MessageCallback& cb);
    void setWriteCallback(const MessageCallback& cb);
    void setIp(const std::string& ip);
    int send(const std::string& msg);
    int fd() const { return channel_.fd(); }
    Channel* channel() { return &channel_; }
    EventLoop* loop() const { return loop_; }
    void handleClose();
private:
    EventLoop* loop_;
    Channel channel_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
    std::string ip_;
    void handleRead();
    void handleWrite();
    MessageCallback readCallback_;
    MessageCallback writeCallback_;
};

#endif