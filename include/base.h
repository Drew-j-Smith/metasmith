#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <optional>
#include <string_view>
#include <type_traits>

namespace metasmith {

template <int index> struct IndexHolder {};

template <typename Derived> class Base {
private:
    template <typename ptr_type, int index> struct FieldImpl {
        const std::string_view key;
        ptr_type Derived::*const ptr;

        template <int idx = index>
        requires(idx == index) constexpr auto get_ptr() const { return ptr; }

        template <int idx = index>
        requires(idx == index) constexpr auto get_key() const { return key; }
    };

    template <typename... Args> struct FieldCollection : public Args... {
        using Args::get_ptr...;
        using Args::get_key...;

        constexpr FieldCollection(auto &&...args) : Args(args)... {}
    };

public:
    template <typename... Args, typename PtrType>
    consteval static auto gen_fields(const std::string_view key,
                                     PtrType Derived::*data, Args &&...args) {
        return gen_fields(IndexHolder<0>{}, key, data, args...);
    }

    template <typename... Args, typename PtrType, int index>
    consteval static auto gen_fields(IndexHolder<index>,
                                     const std::string_view key,
                                     PtrType Derived::*data, Args &&...args) {
        return gen_fields(IndexHolder<index + 1>{}, args...,
                          make_record<index>(key, data));
    }

    template <typename... Args, typename PtrType, int index>
    consteval static auto gen_fields(IndexHolder<index>,
                                     FieldImpl<PtrType, 0> first,
                                     Args &&...args) {
        return FieldCollection<FieldImpl<PtrType, 0>, Args...>{first, args...};
    }

    template <int index, typename ptr_type>
    consteval static auto make_record(const std::string_view key,
                                      ptr_type Derived::*data) {
        return FieldImpl<ptr_type, index>{key, data};
    }
};

} // namespace metasmith
