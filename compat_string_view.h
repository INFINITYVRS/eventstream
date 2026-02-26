// Minimal compatibility shim for std::string_view when the standard header is unavailable.
#pragma once

#if defined(__has_include)
#  if __has_include(<string_view>)
#    include <string_view>
#    define EVENTSTREAM_HAVE_STD_STRING_VIEW 1
#  endif
#endif

#if !defined(EVENTSTREAM_HAVE_STD_STRING_VIEW)
#include <string>
#include <cstring>
#include <cstddef>

namespace std {
    // Very small subset of std::string_view used for compatibility only.
    class string_view {
    public:
        using size_type = std::size_t;

        string_view() noexcept : data_(nullptr), size_(0) {}
        string_view(const char* s) noexcept : data_(s), size_(s ? std::strlen(s) : 0) {}
        string_view(const std::string& s) noexcept : data_(s.data()), size_(s.size()) {}

        const char* data() const noexcept { return data_; }
        size_type size() const noexcept { return size_; }
        const char* begin() const noexcept { return data_; }
        const char* end() const noexcept { return data_ + size_; }
        bool empty() const noexcept { return size_ == 0; }

    private:
        const char* data_;
        size_type size_;
    };
}
#endif
