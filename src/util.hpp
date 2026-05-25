#pragma once
#include <peel/String.h>
#include <format>
#include <iterator>
#include <string>
#include <string_view>

inline std::string_view to_sview(const peel::String& s) noexcept
{
    auto p = s.c_str();
    return p ? std::string_view{p} : std::string_view{};
}

inline bool is_empty(const peel::String& s) noexcept
{
    auto p = s.c_str();
    return p == nullptr || *p == '\0';
}

template<>
struct std::formatter<peel::String> : std::formatter<std::string_view> {
    auto format(const peel::String& s, auto& ctx) const {
        return std::formatter<std::string_view>::format(
            s.c_str() ? std::string_view{s.c_str()} : std::string_view{}, ctx);
    }
};

inline peel::String copy_to_peel_string(std::string_view sv) noexcept
{
    return peel::String(sv.data(), sv.size());
}

inline peel::String copy_to_peel_string(const std::string& s) noexcept
{
    return copy_to_peel_string(std::string_view{s});
}

template<typename... Args>
peel::String format_to_peel(std::format_string<Args...> fmt, Args&&... args)
{
    struct buf_t {
        char*       data = nullptr;
        std::size_t len  = 0;
        std::size_t cap  = 0;
        ~buf_t() { g_free(data); }
        void push(char c) {
            if (len == cap) {
                cap  = cap ? cap * 2 : 32;
                data = static_cast<char*>(g_realloc(data, cap));
            }
            data[len++] = c;
        }
        char* release() { auto p = data; data = nullptr; return p; }
    } buf;
    struct out_it {
        buf_t* b;
        using difference_type [[maybe_unused]] = std::ptrdiff_t;
        struct ref { buf_t* b; void operator=(char c) const { b->push(c); } };
        out_it& operator++()    { return *this; }
        out_it  operator++(int) { return *this; }
        ref     operator*()     { return {b}; }
    };
    static_assert(std::output_iterator<out_it, char>
             && std::is_same_v<std::iter_difference_t<out_it>, std::ptrdiff_t>);
    std::format_to(out_it{&buf}, fmt, std::forward<Args>(args)...);
    buf.push('\0');
    return peel::String::adopt_string(buf.release());
}
