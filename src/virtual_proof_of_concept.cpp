#include <array>
#include <iostream>
#include <optional>

template <typename ClassType, typename PtrType> struct Field_T;

struct Field {
    virtual ~Field() = default;

    template <typename PtrType, typename ClassType>
    std::optional<PtrType> get(ClassType &ref) const {
        if (auto ptr =
                dynamic_cast<const Field_T<ClassType, PtrType> *>(this)) {
            return ref.*(ptr->ptr);
        }
        return {};
    }
};

template <typename ClassType, typename PtrType> struct Field_T : Field {
    PtrType ClassType::*ptr;
    constexpr Field_T(PtrType ClassType::*ptr) : ptr(ptr) {}
};

template <auto ptr> struct StaticField {
    constexpr static auto instance = Field_T{ptr};
};

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
        if (auto x = field->get<int>(s)) {
            std::cout << x.value() << '\n';
        }
    }
}
