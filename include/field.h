#pragma once

#include <any>

namespace metasmith {

template <int index> struct IndexHolder {};

class Field {
private:
    std::string_view key;
    std::any (*get_fp)(const void *);
    void (*set_fp)(void *, const void *);

    template <typename T, int index>
    static std::any get_field(const void *ref) {
        const T *ref_as_t = static_cast<const T *>(ref);
        return ref_as_t->*(ref_as_t->fields.template get_ptr<index>());
    }

    template <typename T, int index>
    static void set_field(void *ref, const void *val) {
        T *ref_as_t = static_cast<T *>(ref);
        auto *actual_val = static_cast<const std::remove_reference_t<
            decltype(ref_as_t->*(ref_as_t->fields.template get_ptr<index>()))>
                                           *>(val);
        ref_as_t->*(ref_as_t->fields.template get_ptr<index>()) = *actual_val;
    }

public:
    constexpr Field() : key(), get_fp(nullptr), set_fp(nullptr) {}

    template <typename T, int index>
    constexpr Field(T *, IndexHolder<index>, std::string_view key)
        : key(key), get_fp(get_field<T, index>), set_fp(set_field<T, index>) {}

    template <typename T> std::any get(const T &ref) const {
        return get_fp(&ref);
    }
    template <typename T, typename S> void set(T &ref, const S &val) {
        set_fp(&ref, &val);
    }

    constexpr std::string_view get_key() { return key; }
};
} // namespace metasmith