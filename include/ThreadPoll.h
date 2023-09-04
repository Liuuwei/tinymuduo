//
// Created by Liuwei on 2023/8/6.
//

#ifndef WEBSERVER_THREADPOLL_H
#define WEBSERVER_THREADPOLL_H

#include "Thread.h"

#include <vector>
#include <mutex>

class ThreadPoll {
public:
    explicit ThreadPoll(int nums, EventLoop* loop);
    ~ThreadPoll();
    EventLoop* getOneLoop();
private:
    int mark_;
    int nums_;
    std::mutex mutex_;
    EventLoop *loop_;
    std::binary_semaphore sem_;
    std::vector<Thread*> threads_;
    std::vector<EventLoop*> loops_;
};

#endif //WEBSERVER_THREADPOLL_H
