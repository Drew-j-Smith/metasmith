#pragma once

#include "base.h"

namespace metasmith {

template <typename Derived> struct setter : base<Derived> {
    template <typename val_type>
    constexpr void set(const std::string_view str, val_type &&val) {
        Derived *derived_this = static_cast<Derived *>(this);
        Derived::impl([&, val = std::forward<val_type>(val)](auto &&...args) {
            return std::array{[&]<typename record_type>(record_type obj) {
                if constexpr (asignable<val_type, typename record_type::type>) {
                    if (obj.get_key() == str) {
                        derived_this->*obj.ptr = val;
                    }
                }
                return 0;
            }(args)...};
        });
    }
};

} // namespace metasmith