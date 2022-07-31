#include "base.h"

#include <any>

namespace metasmith {

template <int index> struct index_holder {};

template <typename T, int index> std::any get_field(const void *ref) {
    const T *ref_as_t = static_cast<const T *>(ref);
    return ref_as_t->*(ref_as_t->fields.template get_ptr<index>());
}

template <typename T, int index> void set_field(void *ref, const void *val) {
    T *ref_as_t = static_cast<T *>(ref);
    auto *actual_val = static_cast<const std::remove_reference_t<
        decltype(ref_as_t->*(ref_as_t->fields.template get_ptr<index>()))> *>(
        val);
    ref_as_t->*(ref_as_t->fields.template get_ptr<index>()) = *actual_val;
}

class Field {
    std::any (*get_fp)(const void *);
    void (*set_fp)(void *, const void *);
    const std::type_info &class_type;
    const std::type_info &val_type;

public:
    template <typename T, int index>
    constexpr Field(T *class_ptr, index_holder<index>)
        : get_fp(get_field<T, index>), set_fp(set_field<T, index>),
          class_type(typeid(T)),
          val_type(
              typeid(decltype(class_ptr->*(class_ptr->fields
                                               .template get_ptr<index>())))) {}

    template <typename T> std::any get(const T &ref) const {
        if (class_type != typeid(T)) {
            throw std::exception{};
        }
        return get_fp(&ref);
    }
    template <typename T, typename S> void set(T &ref, const S &val) {
        if (class_type != typeid(T) || val_type != typeid(S)) {
            throw std::exception{};
        }
        set_fp(&ref, &val);
    }
};
} // namespace metasmith