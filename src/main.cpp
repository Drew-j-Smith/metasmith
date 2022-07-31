
#include "base.h"

#include <iostream>

using namespace std::literals;

struct S : metasmith::Base<S> {
    int m_int;
    float m_float;

    static constexpr auto fields =
        gen_fields("int", &S::m_int, "float", &S::m_float);
};

int main() {

    S s;
    s.m_int = 1;

    for (metasmith::Field field : S::get_fields()) {
        if (field.get_key() == "int"sv) {
            std::cout << field.get<int>(s) << '\n';
        }
    }
}