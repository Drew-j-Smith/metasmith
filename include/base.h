#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <optional>
#include <string_view>
#include <type_traits>

namespace metasmith {

template <typename Derived> class base {
private:
    constexpr Derived &derived() { return *static_cast<Derived *>(this); }

    constexpr const Derived &derived() const {
        return *static_cast<const Derived *>(this);
    }

    template <typename ptr_type, const std::string_view &key> struct record {
        using type = ptr_type;
        ptr_type Derived::*ptr;

        constexpr record(auto ptr) : ptr{ptr} {}

        static constexpr auto get_key() { return key; }
    };

public:
    consteval static auto generator(auto &&...args) {
        return [=](auto &&callable) { return std::invoke(callable, args...); };
    }

    template <const std::string_view &key, typename ptr_type>
    consteval static auto make_record(ptr_type Derived::*data) {
        return record<ptr_type, key>{data};
    }

    template <typename val_type>
    constexpr void set(const std::string_view str, val_type &&val) {
        Derived::impl([&](auto &&...args) {
            return std::array{[&]<typename record_type>(record_type obj) {
                if constexpr (std::is_convertible_v<
                                  val_type, typename record_type::type>) {
                    if (obj.get_key() == str) {
                        derived().*obj.ptr =
                            static_cast<typename record_type::type>(val);
                    }
                }
                return 0;
            }(args)...};
        });
    }

    constexpr void set(auto &&...objs) {
        std::size_t idx{};
        std::string_view curr;
        [&] {
            return std::array{[&]<typename obj_type>(obj_type &&obj) {
                if (idx % 2 == 0) {
                    if constexpr (std::is_convertible_v<
                                      std::remove_reference_t<obj_type>,
                                      std::string_view>) {
                        curr = static_cast<std::string_view>(obj);
                    }
                } else {
                    derived().set(curr, obj);
                }
                ++idx;
                return 0;
            }(objs)...};
        }();
    }

    template <typename val_type>
    constexpr auto get(const std::string_view str) const {
        std::optional<val_type> res;
        Derived::impl([&](auto &&...args) {
            return std::array{[&]<typename record_type>(record_type obj) {
                if constexpr (std::is_convertible_v<typename record_type::type,
                                                    val_type>) {
                    if (obj.get_key() == str) {
                        res = static_cast<val_type>(derived().*obj.ptr);
                    }
                }
                return 0;
            }(args)...};
        });

        return res;
    }
};

} // namespace metasmith
