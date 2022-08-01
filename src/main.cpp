
#include <array>
#include <iostream>
#include <string_view>
#include <type_traits>

using namespace std::literals;

template <auto ptr> struct FieldRef {};

class Field {
private:
    std::string_view key;
    const void *(*get_fp)(const void *);
    void (*set_fp)(void *, const void *);

    template <typename T, typename PtrType, PtrType T::*ptr>
    static const void *get_field(const void *ref) {
        return &(static_cast<const T *>(ref)->*ptr);
    }

    template <typename T, typename PtrType, PtrType T::*ptr>
    static void set_field(void *ref, const void *val) {
        static_cast<T *>(ref)->*ptr = *static_cast<const PtrType *>(val);
    }

public:
    template <typename T, typename PtrType, PtrType T::*ptr>
    constexpr Field(std::string_view key, FieldRef<ptr>)
        : key(key), get_fp(get_field<T, PtrType, ptr>),
          set_fp(set_field<T, PtrType, ptr>) {}

    template <typename PtrType, typename T> PtrType get(const T &ref) const {
        return *static_cast<const PtrType *>(get_fp(&ref));
    }

    template <typename T, typename PtrType>
    void set(T &ref, const PtrType &val) {
        set_fp(&ref, &val);
    }

    constexpr auto get_key() { return key; }
};

struct S {
    int m_int;
    float m_float;

    constexpr static auto fields =
        std::array{Field{"int"sv, FieldRef<&S::m_int>{}},
                   Field{"float"sv, FieldRef<&S::m_float>{}}};
};

int main() {
    S s;

    for (auto field : S::fields) {
        if (field.get_key() == "int"sv) {
            field.set(s, 1);
            std::cout << field.get<int>(s) << '\n';
        }
    }
}