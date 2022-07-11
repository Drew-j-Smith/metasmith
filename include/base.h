#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <optional>
#include <string_view>
#include <type_traits>

namespace metasmith {

template <typename Derived> struct base {
    template <typename ptr_type, const std::string_view &key> struct record {
        using type = ptr_type;
        ptr_type Derived::*ptr;

        constexpr record(auto ptr) : ptr{ptr} {}

        static constexpr auto get_key() { return key; }
    };

    consteval static auto generator(auto &&...args) {
        return [=](auto &&callable) { return std::invoke(callable, args...); };
    }

    template <const std::string_view &key, typename ptr_type>
    consteval static auto make_record(ptr_type Derived::*data) {
        return record<ptr_type, key>{data};
    }
};

} // namespace metasmith
