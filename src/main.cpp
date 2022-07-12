
#include "base.h"

#include <iostream>
// #include "metasmith.h"

using namespace std::literals;

/*
struct S : serializable<S> {
    int m_int;
    float m_float;

    static constexpr auto int_str{"int"sv};
    static constexpr auto float_str{"float"sv};

    static constexpr auto impl = generator(make_record<int_str>(&S::m_int),
                                           make_record<float_str>(&S::m_float));
};

constexpr auto foo() {
    S s{};
    s.set("int", 1);
    return s.search<int>("int");
}

constexpr auto bar() {
    S s = S::set_from_keys("int", 1, "float", 2.2f);
    return s.search<float>("float");
}

constexpr auto bar2() {
    S s{};
    s.set("int", 1);
    return s.get_data<S::int_str>();
}

int main() {
    constexpr auto a = foo();
    static_assert(a == 1);

    constexpr auto b = bar();
    static_assert(b == 2.2f);

    constexpr auto c = bar2();
    static_assert(c == 1);

    S s = S::deserialize("int 1 float 2.2");
    s.set("int", 2);
    std::cout << "2 == " << s.search<int>("int") << '\n';
    std::cout << "2 == " << s.get_data<S::int_str>() << '\n';
    std::cout << "2.2 == " << s.get_data<S::float_str>() << '\n';
    std::cout << s.serialize();

    S x = S::set_from_keys("int", 1, "float", 2.2f);
    std::cout << x.serialize();

    // compile time constraints error b/c int is not convertable to string_view
    // S z = S::set_from_keys(1, 1, "float", 2.2f);

    for (auto str : S::get_keys())
        std::cout << str << '\n';
}
*/

struct S : metasmith::base<S> {
    int m_int;
    float m_float;

    static constexpr auto int_str{"int"sv};
    static constexpr auto float_str{"float"sv};

    static constexpr auto impl = generator(make_record<int_str>(&S::m_int),
                                           make_record<float_str>(&S::m_float));
};

constexpr auto constexpr_test() {
    constexpr auto a = [] {
        S s{};
        s.set("int", 1);
        return s.get<int>("int").value();
    }();
    static_assert(a == 1);

    constexpr auto b = [] {
        S s{};
        return s.get<int>("unknown");
    }();
    static_assert(!b.has_value());

    constexpr auto c = [] {
        S s{};
        s.set("float"sv, 2.2, "int"sv, 1);
        return s.m_int;
    }();
    static_assert(c == 1);

    constexpr auto d = [] {
        S s{};
        s.set("float"sv, 2.2, "int"sv, 1);
        return s.m_float;
    }();
    static_assert(d == 2.2f);
}

void runtime_test(int c) {
    S s{};
    s.set("int", c);
    std::printf("%d == %d\n\n", c, s.get<int>("int").value());

    s.set("float"sv, 2.2 + c, "int"sv, c);
    std::printf("%f == %f\n", 2.2 + c, s.get<float>("float").value());
    std::printf("%d == %d\n\n", c, s.get<int>("int").value());
}

int main(int argc, [[maybe_unused]] char **argv) {

    runtime_test(argc);
    constexpr_test();
}