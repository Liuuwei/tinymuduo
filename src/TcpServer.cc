#include "TcpServer.h"

#include <signal.h>
#include <sys/timerfd.h>
#include <netinet/tcp.h>
#include "Log.h"

TcpServer::TcpServer(EventLoop* loop, const InetAddr& addr) : loop_(loop), listenFd_(addr.listenFd()),
                                                        listenChannel_(std::make_shared<Channel>(loop, listenFd_)),
                                                        threadNums_(0), threadPoll_(nullptr), nagle_(true),
                                                        maxBytes_(INT_MAX)
                                                        {
    signal(SIGPIPE, SIG_IGN);
    acceptor_.setListenFd(listenFd_);
}

TcpServer::~TcpServer() {
    if (threadPoll_) {
        delete threadPoll_;
        threadPoll_ = nullptr;
    }
}

void TcpServer::start() {
    listenChannel_->setReadCallback([this] { newTcpConnection(); });
    listenChannel_->enableRead();
    loop_->updateInLoop(listenChannel_);
    std::thread t([&]() {
        Log::Instance()->start();
    });
    t.detach();
}

void TcpServer::setOnMessage(const MessageCallback & cb) {
    onMessageCallback_ = cb;
}

void TcpServer::setOnConnection(const std::function<void(const std::shared_ptr<TcpConnection> &)> &cb) {
    onConnection_ = cb;
}

void TcpServer::setThreadNums(int nums) {
    threadNums_ = nums;
    threadPoll_ = new ThreadPoll(threadNums_, loop_);
    loops_.reserve(nums);
    for (int i = 0; i < nums; i++) {
        loops_.push_back(threadPoll_->getOneLoop());
    }
    loops_.push_back(loop_);
}

void TcpServer::newTcpConnection() {
    auto tcp = acceptor_.acceptNew();
    int fd = tcp.first;
    if (fd == -1) { return; }
    if (!nagle_) {
        int flag = 1;
        setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    }
    auto loop = threadPoll_->getOneLoop();
    std::shared_ptr<TcpConnection> conn = std::make_shared<TcpConnection>(loop, fd, tcp.second);
    loop->inertToTimeWheel(conn);
    loop->insertNewConnection(conn);
    conn->setReadCallback(onMessageCallback_);
    conn->start();
    tcpConnections_.push_back(conn);
    if (onConnection_) {
        onConnection_(conn);
    }
}