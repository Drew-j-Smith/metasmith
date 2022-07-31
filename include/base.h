#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <optional>
#include <string_view>
#include <type_traits>

namespace metasmith {

template <typename Derived> class Base {
private:
    constexpr Derived &derived() { return *static_cast<Derived *>(this); }

    constexpr const Derived &derived() const {
        return *static_cast<const Derived *>(this);
    }

    template <typename ptr_type, const std::string_view &key,
              ptr_type Derived::*ptr, int index>
    struct record {
        using type = ptr_type;

        template <int idx = index>
        requires(idx == index) static constexpr auto get_ptr() { return ptr; }

        template <int idx = index>
        requires(idx == index) static constexpr auto get_key() { return key; }
    };

    template <typename... Args> struct FieldCollection : public Args... {
        using Args::get_ptr...;
        using Args::get_key...;

        constexpr FieldCollection() = default;
    };

public:
    template <typename... Args>
    consteval static auto generator(Args &&...args) {
        return [=](auto &&callable) { return std::invoke(callable, args...); };
    }

    template <typename... Args>
    consteval static auto gen_fields(Args &&...args) {
        return FieldCollection<Args...>{};
    }

    template <const std::string_view &key, typename ptr_type,
              ptr_type Derived::*data, int index>
    consteval static auto make_record() {
        return record<ptr_type, key, data, index>{};
    }

    template <typename val_type>
    constexpr void set(const std::string_view str, val_type &&val) {
        Derived::impl([&](auto &&...args) {
            return std::array{[&]<typename record_type>(record_type obj) {
                if constexpr (std::is_convertible_v<
                                  val_type, typename record_type::type>) {
                    if (obj.get_key() == str) {
                        derived().*obj.get_ptr() =
                            static_cast<typename record_type::type>(val);
                    }
                }
                return 0;
            }(args)...};
        });
    }

    template <typename val_type, typename... obj_type>
    constexpr void set(const std::string_view str, val_type &&val,
                       obj_type &&...objs) {
        set(str, std::forward<val_type>(val));
        set(std::forward<obj_type>(objs)...);
    }

    template <typename val_type>
    constexpr auto get(const std::string_view str) const {
        std::optional<val_type> res;
        Derived::impl([&](auto &&...args) {
            return std::array{[&]<typename record_type>(record_type obj) {
                if constexpr (std::is_convertible_v<typename record_type::type,
                                                    val_type>) {
                    if (obj.get_key() == str) {
                        res = static_cast<val_type>(derived().*obj.get_ptr());
                    }
                }
                return 0;
            }(args)...};
        });

        return res;
    }
};

} // namespace metasmith
