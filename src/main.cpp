
#include "base.h"
#include "field.h"

#include <iostream>

using namespace std::literals;

struct S : metasmith::Base<S> {
    int m_int;
    float m_float;

    static constexpr auto fields =
        gen_fields("int", &S::m_int, "float", &S::m_float);
};

int main(int argc, [[maybe_unused]] char **argv) {

    S s{};
    metasmith::Field field(&s, metasmith::IndexHolder<0>{});
    field.set(s, 1);

    std::cout << std::any_cast<int>(field.get(s)) << '\n';
}