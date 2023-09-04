#include "EventLoop.h"
#include "Log.h"

#include <sys/eventfd.h>
#include <sys/types.h>
#include <sys/timerfd.h>

EventLoop::EventLoop() : threadId_(gettid()),
                         wakeUpFd_(eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)), wakeUpChannel_(this, wakeUpFd_),
                         everyFd_(timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK)), everyChannel_(this, everyFd_),
                         connectionQueue_(8) {
    wakeUpChannel_.setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeUpChannel_.enableRead();
    poll_.updateChannel(&wakeUpChannel_);
}

EventLoop::~EventLoop() {

}

void EventLoop::loop() {
    while (true) {
        poll_.poll();
        doFunctors();
    }
}

void EventLoop::runInLoop(Functor cb) {
    std::unique_lock lock(funMutex_);
    functors_.push_back(std::move(cb));
    if (!isInLoopThread()) {
        wakeUp();
    }
}

void EventLoop::updateInLoop(Channel* channel) {
    if (isInLoopThread()) {
        poll_.updateChannel(channel);
    } else {
        runInLoop(std::bind(&Poll::updateChannel, &poll_, channel));
    }
}

bool EventLoop::isInLoopThread() const {
    return threadId_ == gettid();
}

void EventLoop::handleRead() const {
    uint64_t one = 1;
    ssize_t n = read(wakeUpFd_, &one, sizeof(one));
}

void EventLoop::wakeUp() const {
    uint64_t one = 1;
    ssize_t n = write(wakeUpFd_, &one, sizeof(one));
}

void EventLoop::doFunctors() {
    std::unique_lock lock(funMutex_);
    std::vector<std::function<void()>> newFunctors;
    functors_.swap(newFunctors);
    lock.unlock();
    for (const auto & newFunctor : newFunctors) {
        newFunctor();
    }
}

void EventLoop::runEvery(int time, std::function<void()> cb) {
    itimerspec timer{};
    timer.it_interval.tv_sec = time;
    timer.it_value.tv_sec = time;
    timerfd_settime(everyFd_, 0, &timer, nullptr);
    everyChannel_.setReadCallback([this, cb]() {
        uint64_t one;
        int n = read(this->everyFd_, &one, sizeof(one));
        if (cb) {
            cb();
        }
    });
    everyChannel_.enableRead();
    updateInLoop(&everyChannel_);
}

void EventLoop::clear() {
    connectionQueue_.push(Bucket());
}