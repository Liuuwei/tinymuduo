#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "TcpServer.h"
#include "Poll.h"
#include "Channel.h"
#include "TcpConnection.h"
#include "CircularQueue.h"

#include <memory>
#include <functional>
#include <mutex>
#include <unordered_set>

class EventLoop {
public:
    typedef std::function<void()> Functor;
    typedef std::unordered_set<std::shared_ptr<TcpConnection>> Bucket;
public:
    EventLoop();
    ~EventLoop();
    void loop();
    void runInLoop(Functor cb);
    void updateInLoop(std::shared_ptr<Channel> channel);
    pid_t LoopThreadId() const {
        return threadId_;
    }
    void runEvery(int time, std::function<void()> cb);
    void clear();
    CircularQueue<Bucket> connectionQueue();
    std::unordered_map<int, std::weak_ptr<TcpConnection>> tcpConnections();
    void insertNewConnection(const std::shared_ptr<TcpConnection>& conn);
    void inertToTimeWheel(std::shared_ptr<TcpConnection> conn);
private:
    Poll poll_;
    pid_t threadId_;
    std::mutex funMutex_;
    std::mutex tcpMutex_;
    int wakeUpFd_;
    std::shared_ptr<Channel> wakeUpChannel_;
    std::vector<Functor> functors_;
    std::unordered_map<int, std::weak_ptr<TcpConnection>> tcpConnections_;
    std::function<void()> timerCallback_;
    bool isInLoopThread() const;
    void handleRead() const;
    void wakeUp() const;
    void doFunctors();
    int everyFd_;
    std::shared_ptr<Channel> everyChannel_;
    CircularQueue<Bucket> connectionQueue_;
};

#endif