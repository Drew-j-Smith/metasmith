#pragma once

#include "base.h"

namespace metasmith {

template <typename Derived> struct setter : base<Derived> {
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
};

} // namespace metasmith