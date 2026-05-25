#pragma once
#include <string>
#include <string_view>
#include <peel/GLib/Variant.h>
#include "util.hpp"

// Accepts peel::GLib::Variant*, RefPtr<Variant>, or FloatPtr<Variant> — all convert to Variant*.
inline std::string_view variant_to_string_view(auto &&v) {
    peel::GLib::Variant *raw = v;
    const char *s = raw->get<const char*>();
    return s ? std::string_view{s} : std::string_view{};
}

inline peel::String variant_to_peel_string(auto &&v) {
    return copy_to_peel_string(variant_to_string_view(v));
}

inline std::string variant_to_string(auto &&v) {
    return std::string{variant_to_string_view(v)};
}
