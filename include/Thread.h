//
// Created by Liuwei on 2023/8/6.
//

#ifndef WEBSERVER_THREAD_H
#define WEBSERVER_THREAD_H


#include <semaphore>

class EventLoop;

class Thread {
public:
    Thread(EventLoop** loop, std::binary_semaphore& sem);
private:
};

#endif //WEBSERVER_THREAD_H
