#include "astream.h"

#include <mutex>
#include <iostream>


aout::aout() : out_(std::cout) {}
aout::aout(std::ostream& out) : out_(out) {}
aout::~aout() { flush(); }

void aout::flush() {
    static std::mutex mut_;
    std::lock_guard<std::mutex> guard(mut_);
    out_ << buffer_.str();
    buffer_.clear();
}

aout& aout::operator<<(aout::endl_t endl) {
    endl(buffer_);
    return *this;
}
