// Helper function to print containers

#include <sstream>
#include <vector>


template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    if (v.empty()) {
        return os << "[]";
    }

    os << "[" << v.front();
    for (auto it = v.begin() + 1; it != v.end(); ++it) {
        os << ", " << *it;
    }
    return os << "]";
}


template<typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::pair<K, V>& kv) {
    return os << kv.first << ": " << kv.second;
}
