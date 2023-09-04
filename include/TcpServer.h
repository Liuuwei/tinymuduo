#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "EventLoop.h"
#include "InetAddr.h"
#include "Acceptor.h"
#include "Buffer.h"
#include "Channel.h"
#include "ThreadPoll.h"
#include "TcpConnection.h"
#include "CircularQueue.h"

#include <memory>
#include <deque>
#include <unordered_set>

class EventLoop;

class TcpServer {
public:
    typedef std::unordered_set<std::shared_ptr<TcpConnection>> Bucket;
    typedef std::function<void(const std::shared_ptr<TcpConnection>&, Buffer*)> MessageCallback;
public:
    TcpServer(EventLoop* loop, InetAddr addr);
    ~TcpServer();
    void start();
    void closeNagle() { nagle_ = false;}
    void setThreadNums(int nums);
    void setOnMessage(const MessageCallback& cb);
    void setOnConnection(const std::function<void(const std::shared_ptr<TcpConnection>&)>& cb);
    std::vector<EventLoop*>& loops() { return loops_; }
    std::vector<std::weak_ptr<TcpConnection>>& tcpConnections() { return tcpConnections_; }
private:
    EventLoop* loop_;
    Acceptor acceptor_;
    int listenFd_;
    int threadNums_;
    Channel* listenChannel_;
    MessageCallback onMessageCallback_;
    std::function<void(const std::shared_ptr<TcpConnection>&)> onConnection_;
    std::vector<std::weak_ptr<TcpConnection>> tcpConnections_;
    ThreadPoll* threadPoll_;
    std::mutex mutex_;
    std::vector<EventLoop*> loops_;
    void newTcpConnection();
    bool nagle_;
};

#endif