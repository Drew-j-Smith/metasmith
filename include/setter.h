#pragma once

#include "base.h"

namespace metasmith {

template <typename Derived> class setter : public base<Derived> {

private:
    template <typename ptr_type> struct setter_impl {
        const std::string_view str;
        ptr_type val;
        Derived *ref;

        constexpr setter_impl(std::string_view str, const ptr_type &val,
                              Derived *ref)
            : str(str), val(val), ref(ref) {}

        template <typename record_type>
        requires asignable<ptr_type, typename record_type::type>
        constexpr friend setter_impl &operator+(setter_impl &s,
                                                record_type obj) {
            if (obj.get_key() == s.str) {
                s.ref->*obj.ptr = s.val;
            }
            return s;
        }

        constexpr friend setter_impl &operator+(setter_impl &s,
                                                [[maybe_unused]] auto obj) {
            return s;
        }
    };

public:
    template <typename ptr_type>
    constexpr void set(const std::string_view str, const ptr_type &val) {
        setter_impl<ptr_type> s{str, val, static_cast<Derived *>(this)};
        Derived::impl(fold(s));
    }
};

} // namespace metasmith