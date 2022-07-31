#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <optional>
#include <string_view>
#include <type_traits>

namespace metasmith {

template <int index> struct IndexHolder {};

class Field {
private:
    std::string_view key;
    const void *(*get_fp)(const void *);
    void (*set_fp)(void *, const void *);

    template <typename T, int index>
    static const void *get_field(const void *ref) {
        const T *ref_as_t = static_cast<const T *>(ref);
        return &(ref_as_t->*(ref_as_t->fields.template get_ptr<index>()));
    }

    template <typename T, int index, typename PtrType>
    static void set_field(void *ref, const void *val) {
        T *ref_as_t = static_cast<T *>(ref);
        ref_as_t->*(ref_as_t->fields.template get_ptr<index>()) =
            *static_cast<const PtrType *>(val);
    }

public:
    constexpr Field() : key(), get_fp(nullptr), set_fp(nullptr) {}

    template <typename T, typename PtrType, int index>
    constexpr Field(PtrType T::*, IndexHolder<index>, std::string_view key)
        : key(key), get_fp(get_field<T, index>),
          set_fp(set_field<T, index, PtrType>) {}

    template <typename S, typename T> S get(const T &ref) const {
        return *static_cast<const S *>(get_fp(&ref));
    }
    template <typename T, typename S> void set(T &ref, const S &val) {
        set_fp(&ref, &val);
    }

    constexpr std::string_view get_key() { return key; }
};
} // namespace metasmith