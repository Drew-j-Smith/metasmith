
#include "metasmith.h"

struct S : serializable<S> {
    int m_int;
    float m_float;

    static constexpr std::string_view int_str{"int"};
    static constexpr std::string_view float_str{"float"};

    static constexpr auto impl =
        generator(s_obj<int, int_str, &S::m_int>{},
                  s_obj<float, float_str, &S::m_float>{});
};

constexpr auto foo() {
    S s{};
    s.set("int", 1);
    // static_assert(err == 1);
    return s.search<int>("int");
}

constexpr auto bar() {
    S s = S::set_from_keys("int", 1, "float", 2.2f);
    return s.search<float>("float");
}

template <std::string_view &str> void test() {}

int main() {
    constexpr auto a = foo();
    static_assert(a == 1);

    constexpr auto b = bar();
    static_assert(b == 2.2f);

    S s = S::deserialize("int 1 float 2.2");
    s.set("int", 2);
    std::cout << "2 == " << s.search<int>("int") << '\n';
    static constexpr std::string_view int_str{"int"};
    static constexpr std::string_view float_str{"float"};
    std::cout << "2 == " << s.get_data<int_str>() << '\n';
    std::cout << "2.2 == " << s.get_data<float_str>() << '\n';
    std::cout << s.serialize();

    S x = S::set_from_keys("int", 1, "float", 2.2f);
    std::cout << x.serialize();

    for (auto str : S::get_keys())
        std::cout << str << '\n';
}