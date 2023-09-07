#include "InetAddr.h"
#include "EventLoop.h"
#include <thread>
#include "Log.h"


unsigned long long bytes = 0;

void onMessage(const std::shared_ptr<TcpConnection>& conn, Buffer* buffer) {
    while (buffer->readAbleBytes() > 0 && buffer->FindEnd() == -1) {
        bytes += buffer->retriveAll().size();
    }
    if (buffer->FindEnd() != -1) {
        bytes += buffer->retriveAll().size();
        int n = conn->send(std::to_string(bytes));
        std::cout << "all bytes: " << bytes << std::endl;
        std::cout << "send: " << n << std::endl;
    }
}

void onConnection(const std::shared_ptr<TcpConnection>& conn) {

}

int main() {
    Log::Instance()->close();
    EventLoop loop;
    InetAddr addr("10.211.55.3", 9999);
    TcpServer server(&loop, addr);
    server.setOnMessage(onMessage);
    server.setOnConnection(onConnection);
    server.setThreadNums(1);
    server.start();
    loop.loop();
}