#include "Buffer.h"
#include "Log.h"

#include <sys/uio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

Buffer::Buffer() : readIndex_(0), writeIndex_(0), buffer_(65536) {

}

Buffer::~Buffer() {

}

int Buffer::readAbleBytes() const {
    return writeIndex_ - readIndex_;
}

int Buffer::writeAblyBytes() const {
    return buffer_.size() - writeIndex_;
}

int Buffer::readIndex() const {
    return readIndex_;
}

int Buffer::writeIndex() const {
    return writeIndex_;
}

void Buffer::append(std::string msg) {
    while (msg.size() > writeAblyBytes()) {
        buffer_.resize(buffer_.size() + (buffer_.size() >> 1));
    }
    std::copy(msg.begin(), msg.end(), buffer_.begin() + writeIndex_);
    writeIndex_ += msg.size();
}

std::string Buffer::retriveAll() {
    auto ret =  std::string(buffer_.begin() + readIndex_, buffer_.begin() + writeIndex_);
    readIndex_ = writeIndex_ = 0;
    return ret;
}

std::string Buffer::retriveSome(int size) {
    std::string tmp = retriveAll();
    std::string ret(tmp.begin(), tmp.begin() + size);
    append(std::string(tmp.begin() + size, tmp.end()));
    return ret;
}

int Buffer::readFd(int fd) {
    char extraBuf[65536];
    struct iovec vec[2];
    const int writeAble = writeAblyBytes();
    vec[0].iov_base = &*buffer_.begin() + writeIndex_;
    vec[0].iov_len = writeAble;
    vec[1].iov_base = extraBuf;
    vec[1].iov_len = sizeof(extraBuf);

    const ssize_t n = ::readv(fd, vec, 2);
    if (n == 0) {
        return 0;
    }
    if (n == -1) {
        return -1;
    }
    if (n > writeAble) {
        std::cout << n << " " << writeAble << std::endl;
        writeIndex_ = buffer_.size();
        append(std::string(extraBuf, n - writeAble));
    } else {
        writeIndex_ += n;
    }
    return n;
}

int Buffer::writeFd(int fd) {
    std::string msg = retriveAll();
    ssize_t n = ::write(fd, &*msg.begin(), msg.size());
    if (n == -1) {
        return -1;
    }
    if (n < msg.size()) {
        append(std::string(msg.begin() + n, msg.end()));
    }
    return n;
}

char Buffer::getChar(int index) {
    if (index < readIndex_ || index > readIndex_ + readAbleBytes()) {
        Log::Instance()->DEBUG("GetChar index out of range");
    }
    return buffer_[index];
}

int Buffer::FindEnd() {
    for (int i = readIndex_; i + 3 < readIndex_ + readAbleBytes(); i++) {
        if (buffer_[i] == '\r' && buffer_[i + 1] == '\n' && buffer_[i + 2] == '\r' && buffer_[i + 3] == '\n') {
            return i + 3;
        }
    }
    return -1;
}