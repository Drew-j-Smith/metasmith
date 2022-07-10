#pragma once

#include "base.h"

namespace metasmith {

template <typename Derived> class setter : public base<Derived> {

private:
    template <typename val_type> struct setter_impl {
        const std::string_view str;
        val_type val;
        Derived &ref;

        template <typename val_type_ctr>
        constexpr setter_impl(const std::string_view str, val_type_ctr &&val,
                              Derived &ref)
            : str(str), val(std::forward<val_type_ctr>(val)), ref(ref) {}

        template <typename record_type>
        requires asignable<val_type, typename record_type::type>
        constexpr friend setter_impl &operator+(setter_impl &s,
                                                record_type obj) {
            if (obj.get_key() == s.str) {
                s.ref.*obj.ptr = s.val;
            }
            return s;
        }

        constexpr friend setter_impl &operator+(setter_impl &s,
                                                [[maybe_unused]] auto obj) {
            return s;
        }
    };

public:
    template <typename val_type>
    constexpr void set(const std::string_view str, val_type &&val) {
        setter_impl<val_type> s{str, std::forward<val_type>(val),
                                *static_cast<Derived *>(this)};
        Derived::impl(fold(s));
    }
};

} // namespace metasmith