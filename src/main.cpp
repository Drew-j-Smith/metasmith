
#include <array>
#include <iostream>
#include <string_view>
#include <type_traits>
#include <unordered_set>

using namespace std::literals;

template <auto ptr> struct FieldRef { std::string_view key; };

template <typename T> class Field {
private:
    std::string_view key;
    const void *(*get_fp)(const T &);
    void (*set_fp)(T &, const void *);
    void (*print_fp)(const T &, std::ostream &);
    std::size_t (*hash_fp)(const T &);

    template <auto ptr> static const void *get_field(const T &ref) {
        return &(ref.*ptr);
    }

    template <typename PtrType, PtrType T::*ptr>
    static void set_field(T &ref, const void *val) {
        ref.*ptr = *static_cast<const PtrType *>(val);
    }

    template <auto ptr>
    static void print_field(const T &ref, std::ostream &out) {
        out << ref.*ptr;
    }

    template <typename PtrType, PtrType T::*ptr>
    static std::size_t hash_field(const T &ref) {
        return std::hash<PtrType>{}(ref.*ptr);
    }

public:
    template <typename PtrType, PtrType T::*ptr>
    constexpr Field(FieldRef<ptr> field)
        : key(field.key), get_fp(get_field<ptr>),
          set_fp(set_field<PtrType, ptr>), print_fp(print_field<ptr>),
          hash_fp(hash_field<PtrType, ptr>) {}

    template <typename PtrType> PtrType get(const T &ref) const {
        return *static_cast<const PtrType *>(get_fp(ref));
    }

    void set(T &ref, const auto &val) { set_fp(ref, &val); }

    void print(const T &ref, std::ostream &out) { print_fp(ref, out); }

    std::size_t hash(const T &ref) { return hash_fp(ref); }

    constexpr auto get_key() { return key; }
};

template <typename T>
concept Base = requires(T t) {
    { t.name } -> std::convertible_to<std::string_view>;
    []<std::size_t size>(std::array<Field<T>, size>) {}(t.fields);
};

std::ostream &operator<<(std::ostream &out, Base auto t) {
    out << t.name << " { ";
    for (auto field : t.fields) {
        out << field.get_key() << ": ";
        field.print(t, out);
        out << ", ";
    }
    return out << "}";
}

constexpr void hash_combine(std::size_t &seed, size_t val) {
    seed ^= val + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <Base T> struct std::hash<T> {
    constexpr std::size_t operator()(T const &t) const {
        std::size_t seed{};
        for (auto field : t.fields) {
            hash_combine(seed, field.hash(t));
        }
        return seed;
    }
};

struct S {
    int m_int;
    float m_float;

    constexpr static auto name = "S";
    constexpr static std::array<Field<S>, 2> fields{
        FieldRef<&S::m_int>{"int"}, FieldRef<&S::m_float>{"float"}};
    constexpr auto operator<=>(const S &) const = default;
};

int main() {
    std::unordered_set<S> set;
    set.insert(S{0, 1.1f});
    set.insert(S{1, 1.4f});
    set.insert(S{2, 1.2f});
    std::cout << set.contains(S{0, 1.1f}) << '\n';
    std::cout << set.contains(S{1, 1.1f}) << '\n';

    S s{0, 1.1f};
    for (auto field : S::fields) {
        if (field.get_key() == "int"sv) {
            field.set(s, 1);
            std::cout << field.get<int>(s) << '\n';
        }
    }
    std::cout << s;
}