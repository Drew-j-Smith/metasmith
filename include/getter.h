#pragma once

#include "base.h"

namespace metasmith {

template <typename Derived> struct getter : base<Derived> {
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