//
// Created by Liuwei on 2023/9/5.
//
#include "EventLoop.h"
#include "TcpClient.h"
#include "Log.h"
#include <iostream>
#include <algorithm>

timeval pre{};

void message(const std::shared_ptr<TcpConnection>& conn, Buffer* buffer) {
    std::cout << "on Message" << std::endl;
    timeval cur{};
    gettimeofday(&cur, nullptr);
    std::string msg = buffer->retriveAll();
    unsigned long long bytes = std::stoi(msg);
    std::cout << "bytes: " << bytes << std::endl;
    int sec = std::max(1, static_cast<int>(cur.tv_sec - pre.tv_sec));
    int mb = (bytes / 1024 / 1024) / sec;
    std::cout << "speed: " << mb << "mb/s" << std::endl;
    exit(0);
}

void connection(const std::shared_ptr<TcpConnection>& conn) {
    gettimeofday(&pre, nullptr);
    std::string msg(1024 * 1024, 'a');
    for (int i = 0; i < 1000; i++) {
        conn->send(msg);
    }
    conn->send("end\r\n\r\n");
    //std::cout << "send all" << std::endl;
}

int main() {
    Log::Instance()->close();
    EventLoop loop;
    TcpClient client(&loop, "10.211.55.3", 9999);
    client.setReadCallback(message);
    client.setOnConnection(connection);
    client.connect();
    loop.loop();
}