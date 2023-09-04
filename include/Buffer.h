#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <string>

class Buffer {
public:
    Buffer();
    ~Buffer();
    int readAbleBytes() const;
    int writeAblyBytes() const;
    int readIndex() const;
    int writeIndex() const;
    void append(std::string msg);
    char getChar(int index);
    std::string retriveAll();
    std::string retriveSome(int size);
    int readFd(int fd);
    int writeFd(int fd);
    int FindEnd();
private:
    int readIndex_;
    int writeIndex_;
    std::vector<char> buffer_;
};

#endif