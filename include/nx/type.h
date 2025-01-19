#pragma once

#include <string.h>

#include <cstddef>
#include <cstdint>

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <variant>
#include <chrono>

namespace nx {

using ByteBuffer = std::vector<uint8_t>;

template <class T>
using Optional = std::optional<T>;

using String = std::string;

/**
 * @brief      用于被private继承, 子类将不可被Copy, 不可被Move
 */
class Uncopyable {
public:
    Uncopyable() = default;
    ~Uncopyable() = default;

    Uncopyable(const Uncopyable&) = delete;
    Uncopyable& operator=(const Uncopyable&) = delete;

    Uncopyable(Uncopyable&&) = delete;
    Uncopyable& operator=(Uncopyable&&) = delete;
};

constexpr size_t operator""_kb(unsigned long long int n) { return n * 1024; }

template <class FROM, class TO>
FROM from(TO);

template <class T>
String to_string(const T&);

} // namespace nx
