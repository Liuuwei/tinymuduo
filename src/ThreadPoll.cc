#include "ThreadPoll.h"
#include "Util.h"
#include "EventLoop.h"

ThreadPoll::ThreadPoll(int nums, EventLoop* loop) : mark_(0), nums_(nums), threads_(nums), loops_(nums), sem_(0), loop_(loop){
    for (int i = 0; i < nums; i++) {
        EventLoop* curLoop = nullptr;
        auto thread = new Thread(&curLoop, sem_);
        threads_[i] = thread;
        loops_[i] = curLoop;
    }
}

ThreadPoll::~ThreadPoll() {
    for (int i = 0; i < nums_; i++) {
        if (threads_[i]) {
            delete threads_[i];
            threads_[i] = nullptr;
        }
    }
}

EventLoop* ThreadPoll::getOneLoop() {
    if (nums_ == 0) {
        return loop_;
    }
    EventLoop* loop = loops_[mark_];
    mark_ = (mark_ + 1) % nums_;
    return loop;
}