#pragma once

#include <functional>
#include <string_view>
#include <type_traits>

namespace metasmith {

constexpr auto fold(auto &consumer) {
    return [&](auto &&...args) { (consumer + ... + args); };
}

/**
 * @brief essentially convertible_to but disable integral conversion
 */
template <typename T, typename S>
concept asignable = requires(const T &t) {

    { t } -> std::convertible_to<std::remove_reference_t<S>>;
    requires(std::is_integral_v<std::remove_reference_t<T>> &&
             std::is_same_v<std::remove_reference_t<T>,
                            std::remove_reference_t<S>>) ||
        !std::is_integral_v<std::remove_reference_t<T>>;
};

template <typename Derived> class base {
public:
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
