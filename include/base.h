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

    template <typename val_type>
    constexpr void set(const std::string_view str, val_type &&val) {
        Derived *derived_this = static_cast<Derived *>(this);
        Derived::impl([&](auto &&...args) {
            return std::array{[&]<typename record_type>(record_type obj) {
                if constexpr (std::is_convertible_v<
                                  val_type, typename record_type::type>) {
                    if (obj.get_key() == str) {
                        derived_this->*obj.ptr =
                            static_cast<typename record_type::type>(val);
                    }
                }
                return 0;
            }(args)...};
        });
    }

    template <typename val_type>
    constexpr auto get(const std::string_view str) const {
        const Derived *derived_this = static_cast<const Derived *>(this);
        std::optional<val_type> res;
        Derived::impl([&](auto &&...args) {
            return std::array{[&]<typename record_type>(record_type obj) {
                if constexpr (std::is_convertible_v<typename record_type::type,
                                                    val_type>) {
                    if (obj.get_key() == str) {
                        res = static_cast<val_type>(derived_this->*obj.ptr);
                    }
                }
                return 0;
            }(args)...};
        });

        return res;
    }
};

} // namespace metasmith
