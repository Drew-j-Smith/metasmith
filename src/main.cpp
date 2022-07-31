
#include "base.h"
#include "field.h"

#include <iostream>

using namespace std::literals;

struct S : metasmith::Base<S> {
    int m_int;
    float m_float;

    static constexpr auto int_str{"int"sv};
    static constexpr auto float_str{"float"sv};

    static constexpr auto impl =
        generator(make_record<int_str, int, &S::m_int, 0>(),
                  make_record<float_str, float, &S::m_float, 0>());
    static constexpr auto fields =
        gen_fields(make_record<int_str, int, &S::m_int, 0>(),
                   make_record<float_str, float, &S::m_float, 1>());
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

    S s{};
    metasmith::Field field(&s, metasmith::index_holder<0>{});
    field.set(s, 1);

    std::cout << s.m_int << '\n';
}