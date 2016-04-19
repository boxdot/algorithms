#pragma once

#include <ostream>
#include <sstream>


// atomic out stream
class aout {
public:
    aout();
    aout(std::ostream& out);
    ~aout();

    template<typename T>
    aout& operator<<(const T& val) {
        buffer_ << val;
        return *this;
    }

    void flush();

    using cout_t = std::basic_ostream<char, std::char_traits<char>>;
    using endl_t = cout_t&(*)(cout_t&);

    aout& operator<<(endl_t endl);

private:
    std::ostream& out_;
    std::stringstream buffer_;
};
