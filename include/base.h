#pragma once

#include "field.h"

namespace metasmith {

template <typename Derived> class Base {
private:
    template <typename ptr_type, int index> struct FieldImpl {
        const std::string_view key;
        ptr_type Derived::*const ptr;

        template <int idx = index>
        requires(idx == index) constexpr auto get_ptr() const { return ptr; }

        template <int idx = index>
        requires(idx == index) constexpr auto get_key() const { return key; }

        template <int idx = index>
        requires(idx == index) constexpr auto get_field() const {
            return Field{ptr, IndexHolder<index>{}, key};
        }
    };

    template <typename... Args> struct FieldCollection : public Args... {
        using Args::get_ptr...;
        using Args::get_key...;
        using Args::get_field...;

        std::array<Field, sizeof...(Args)> fields;

        template <int index = 0>
        constexpr Field get_field_impl(std::size_t idx) {
            if constexpr (index < sizeof...(Args)) {
                if (idx == 0) {
                    return this->template get_field<index>();
                } else {
                    return get_field_impl<index + 1>(idx - 1);
                }
            } else {
                return Field{};
            }
        }

        constexpr FieldCollection(auto &&...args) : Args(args)... {
            for (std::size_t i = 0; i < sizeof...(Args); i++) {
                fields[i] = get_field_impl(i);
            }
        }
    };

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

public:
    template <typename... Args, typename PtrType>
    consteval static auto gen_fields(const std::string_view key,
                                     PtrType Derived::*data, Args &&...args) {
        return gen_fields(IndexHolder<0>{}, key, data, args...);
    }

    constexpr static auto get_fields() { return Derived::fields.fields; }
};

} // namespace metasmith
