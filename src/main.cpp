
#include <array>
#include <iostream>
#include <string_view>
#include <type_traits>

using namespace std::literals;

template <auto ptr> struct FieldRef { std::string_view key; };

template <typename T> class Field {
private:
    std::string_view key;
    const void *(*get_fp)(const T &);
    void (*set_fp)(T &, const void *);
    void (*print_fp)(T &, std::ostream &);

    template <auto ptr> static const void *get_field(const T &ref) {
        return &(ref.*ptr);
    }

    template <typename PtrType, PtrType T::*ptr>
    static void set_field(T &ref, const void *val) {
        ref.*ptr = *static_cast<const PtrType *>(val);
    }

    template <auto ptr> static void print_field(T &ref, std::ostream &out) {
        out << ref.*ptr;
    }

public:
    template <typename PtrType, PtrType T::*ptr>
    constexpr Field(FieldRef<ptr> field)
        : key(field.key), get_fp(get_field<ptr>),
          set_fp(set_field<PtrType, ptr>), print_fp(print_field<ptr>) {}

    template <typename PtrType> PtrType get(const T &ref) const {
        return *static_cast<const PtrType *>(get_fp(ref));
    }

    template <typename PtrType> void set(T &ref, const PtrType &val) {
        set_fp(ref, &val);
    }

    void print(T &ref, std::ostream &out) { print_fp(ref, out); }

    constexpr auto get_key() { return key; }
};

template <typename T>
concept Base = requires(T t) {
    { t.name } -> std::convertible_to<std::string_view>;
    []<std::size_t size>(std::array<Field<T>, size>) {}(t.fields);
};

template <Base T> std::ostream &operator<<(std::ostream &out, T t) {
    out << t.name << " { ";
    for (auto field : T::fields) {
        out << field.get_key() << ": ";
        field.print(t, out);
        out << ", ";
    }
    return out << "}";
}

struct S {
    int m_int;
    float m_float;

    constexpr static auto name = "S";
    constexpr static std::array<Field<S>, 2> fields{
        FieldRef<&S::m_int>{"int"}, FieldRef<&S::m_float>{"float"}};
};

int main() {
    S s{0, 1.1f};

    for (auto field : S::fields) {
        if (field.get_key() == "int"sv) {
            field.set(s, 1);
            std::cout << field.get<int>(s) << '\n';
        }
    }
    std::cout << s;
}