//
// Created by Liuwei on 2023/8/6.
//

#include "Thread.h"
#include "Log.h"
#include "EventLoop.h"

#include <thread>

Thread::Thread(EventLoop** loop, std::binary_semaphore& sem) {
    std::thread t([&]() {
        Log::Instance()->LOG("Create a new Thread %d", gettid());
        auto loop_ = new EventLoop;
        *loop = loop_;
        sem.release();
        loop_->loop();
    });
    sem.acquire();
    t.detach();
}