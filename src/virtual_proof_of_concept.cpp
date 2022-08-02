
#include <array>
#include <iostream>
#include <optional>


struct Field {
    virtual ~Field() = default;
};

template <typename ClassType, typename PtrType> struct Field_T : Field {
    PtrType ClassType::*ptr;
    constexpr Field_T(PtrType ClassType::*ptr) : ptr(ptr) {}
    virtual ~Field_T() = default;

    constexpr PtrType get_impl(ClassType &ref) const { return ref.*ptr; }
};

template <auto ptr> struct StaticField {
    constexpr static auto instance = Field_T{ptr};
};

template <typename PtrType, typename ClassType>
std::optional<PtrType> get(const Field *field, ClassType &ref) {
    if (auto ptr = dynamic_cast<const Field_T<ClassType, PtrType> *>(field)) {
        return ptr->get_impl(ref);
    }
    return {};
}

struct S {
    int m_int;
    float m_float;
    constexpr static std::array<const Field *, 2> fields{
        &StaticField<&S::m_int>::instance, &StaticField<&S::m_float>::instance};
};

int main() {
    S s;
    s.m_int = 1;
    for (auto field : S::fields) {
        if (auto x = get<int>(field, s)) {
            std::cout << x.value() << '\n';
        }
    }
}
