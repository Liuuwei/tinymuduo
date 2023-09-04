//
// Created by Liuwei on 2023/8/18.
//

#ifndef WEBSERVER_LOG_H
#define WEBSERVER_LOG_H

#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>

#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>

class Log {
public:
    static Log* Instance();
    void start();

    template<typename... Args>
    void LOG(std::string msg = "", Args... args) {
        if (!run) return ;
        msg = "[" + getTime() + "]" + "[" + std::to_string(gettid()) +"] " + "LOG: " + msg + "\n";

        std::string s(1024, '\0');
        int n = snprintf(&*s.begin(), s.size(), msg.c_str(), args...);
        s.resize(n);

        std::unique_lock lock(mutex_);
        logs_.push_back(s);
        cond_.notify_one();
    }

    template<typename... Args>
    void DEBUG(std::string msg = "", Args... args) {
        if (!run) return ;
        msg = "[" + getTime() + "]" + "[" + std::to_string(gettid()) +"] " + "DEBUG: " + msg + "\n";

        std::string s(1024, '\0');
        int n = snprintf(&*s.begin(), s.size(), msg.c_str(), args...);
        s.resize(n);

        std::unique_lock lock(mutex_);
        logs_.push_back(s);
        cond_.notify_one();
    }

    void close() {
        run = false;
    }
private:
    Log();
    ~Log();
    int fd_;
    bool run = true;
    struct stat fileStat_{};
    std::string name = "log/log-";
    std::string time_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::vector<std::string> logs_;
    tm* getTimeOfDay();
    std::string getDate();
    std::string getTime();
};

#endif //WEBSERVER_LOG_H
