//
// Created by Liuwei on 2023/8/28.
//

#ifndef WEBSERVER_TCPCLIENT_H
#define WEBSERVER_TCPCLIENT_H

#include "InetAddr.h"
#include "Connector.h"
#include "EventLoop.h"
#include "TcpConnection.h"

class EventLoop;
class TcpClient {
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void(const TcpConnectionPtr& conn, Buffer* buffer)> MessageCallBack;
public:
    TcpClient(EventLoop* loop, std::string ip, int port);
    ~TcpClient();
    bool connect();
    void start() {}
    void closeNagle() {
        nagle_ = false;
    }
    int send(const std::string& msg);
    void setWriteCallback(const MessageCallBack & cb) { writeCallback_ = cb; }
    void setReadCallback(const MessageCallBack & cb) { readCallback_ = cb; }
    void setOnConnection(const std::function<void(std::shared_ptr<TcpConnection>)>& cb);
private:
    EventLoop *loop_;
    int fd_;
    std::string ip_;
    int port_;
    Connector connector_;
    ThreadPoll threadPoll_;
    std::shared_ptr<TcpConnection> readConn_;
    std::shared_ptr<TcpConnection> writeConn_;
    MessageCallBack writeCallback_;
    MessageCallBack readCallback_;
    std::function<void(std::shared_ptr<TcpConnection>)> onGetConnection_;
    bool nagle_;
};

#endif //WEBSERVER_TCPCLIENT_H
