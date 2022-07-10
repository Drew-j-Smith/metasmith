#pragma once

#include "base.h"
#include <optional>

namespace metasmith {

template <typename Derived> class getter : public base<Derived> {

private:
    template <typename val_type> struct getter_impl {
        const std::string_view str;
        const Derived &ref;
        std::optional<val_type> res{};

        constexpr getter_impl(const std::string_view str, const Derived &ref)
            : str(str), ref(ref) {}

        template <typename record_type>
        requires asignable<typename record_type::type, val_type>
        constexpr friend getter_impl &operator+(getter_impl &s,
                                                record_type obj) {
            if (obj.get_key() == s.str) {
                s.res = s.ref.*obj.ptr;
            }
            return s;
        }

        constexpr friend getter_impl &operator+(getter_impl &s,
                                                [[maybe_unused]] auto obj) {
            return s;
        }
    };

public:
    template <typename val_type>
    constexpr auto get(const std::string_view str) {
        getter_impl<val_type> s{str, *static_cast<const Derived *>(this)};
        Derived::impl(fold(s));
        return s.res;
    }
};

} // namespace metasmith