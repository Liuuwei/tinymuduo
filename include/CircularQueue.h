//
// Created by Liuwei on 2023/9/3.
//

#ifndef WEBSERVER_CIRCULATQUEUE_H
#define WEBSERVER_CIRCULATQUEUE_H

#include <deque>

template<typename T>
class CircularQueue {
public:
    CircularQueue(int size) : size_(size), dQueue(size) {

    }
    void push(T t) {
        dQueue.pop_front();
        dQueue.push_back(std::move(t));
    }
    T& back() {
        return dQueue.back();
    }
private:
    int size_;
    std::deque<T> dQueue;
};

#endif //WEBSERVER_CIRCULATQUEUE_H
