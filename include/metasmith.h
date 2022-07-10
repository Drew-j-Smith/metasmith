
#include <array>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>

template <typename T> struct serializable {
    template <typename S, const std::string_view &key, S T::*data>
    struct s_obj {
        static constexpr auto get_data() { return data; }
        static constexpr auto get_key() { return key; }
    };

    struct deserializer {
        T ref{};
        std::stringstream str;
        constexpr deserializer(const std::string &str) : str(str) {}

        template <typename S, const std::string_view &str, S T::*data>
        friend deserializer &operator+(deserializer &s, s_obj<S, str, data>) {
            auto idx = s.str.view().find(str);
            if (idx != std::string_view::npos) {
                s.str.seekg(static_cast<typename decltype(s.str)::int_type>(
                    idx + str.length() + 1));
                s.str >> s.ref.*data;
            }
            return s;
        }
    };

    static auto deserialize(const std::string &str) {
        deserializer s{str};
        T::impl(fold(s));
        return s.ref;
    }

    struct serializer {
        const T &ref;
        std::stringstream str{};
        constexpr serializer(const T &ref) : ref(ref) {}

        template <typename S, const std::string_view &str, S T::*data>
        friend serializer &operator+(serializer &s, s_obj<S, str, data>) {
            s.str << str << ':' << s.ref.*data << '\n';
            return s;
        }
    };

    std::string serialize() const {
        serializer s{*static_cast<const T *>(this)};
        T::impl(fold(s));
        return s.str.str();
    }

    template <typename S> struct searcher {
        const std::string_view str;
        const T &ref;
        S res{};

        constexpr searcher(std::string_view str, const T &ref)
            : str(str), ref(ref) {}

        template <typename U, const std::string_view &str, U T::*data>
        constexpr friend searcher &operator+(searcher &s, s_obj<U, str, data>) {
            if constexpr (std::is_same_v<S, U>) {
                if (str == s.str) {
                    s.res = s.ref.*data;
                }
            }
            return s;
        }
    };

    template <typename S> constexpr S search(std::string_view str) const {
        searcher<S> s{str, *static_cast<const T *>(this)};
        T::impl(fold(s));
        return s.res;
    }

    template <typename S> struct setter {
        const std::string_view str;
        S val;
        T *ref;

        constexpr setter(std::string_view str, const S &val, T *ref)
            : str(str), val(val), ref(ref) {}

        template <typename U, const std::string_view &str, U T::*data>
        constexpr friend setter &operator+(setter &s, s_obj<U, str, data>) {
            if constexpr (std::is_same_v<S, U>) {
                if (str == s.str) {
                    s.ref->*data = s.val;
                }
            }
            return s;
        }
    };

    template <typename S>
    constexpr void set(std::string_view str, const S &val) {
        setter<S> s{str, val, static_cast<T *>(this)};
        T::impl(fold(s));
    }

    constexpr static auto get_keys() {
        return T::impl([](auto &&...args) {
            return std::array{
                []<typename S, const std::string_view & str, S T::*data>(
                    s_obj<S, str, data>) { return str; }(args)...};
        });
    }

    struct key_setter {
        T &ref{};
    };

    struct value_setter {
        T &ref{};
        std::string_view str;
    };
    template <typename U>
    constexpr friend value_setter operator+(key_setter s, U obj) {
        return value_setter{s.ref, obj};
    }
    template <typename U>
    constexpr friend key_setter operator+(value_setter s, U obj) {
        s.ref.set(s.str, obj);
        return key_setter{s.ref};
    }

    constexpr static auto set_from_keys(auto &&...args) {
        T ref{};
        key_setter s{ref};
        (s + ... + args);
        return ref;
    }

    template <const std::string_view &str> constexpr auto get_data() const {
        return T::impl([=, this]<typename... args>(args...) {
            using type = typename get_data_recursive<str, args...>::type;
            static_assert(!std::is_void_v<type>, "not found");

            auto obj = static_cast<const T *>(this)->template search<type>(str);
            return static_cast<const T *>(this)->*obj.get_data();
        });
    }

    template <const std::string_view &search, typename S, typename... args>
    struct get_data_recursive {
        using recursive = typename get_data_recursive<search, args...>::type;
        using type = typename std::conditional<search == S::get_key(), S,
                                               recursive>::type;
    };

    template <const std::string_view &search, typename S>
    struct get_data_recursive<search, S> {
        using type =
            typename std::conditional<search == S::get_key(), S, void>::type;
    };

    template <const std::string_view &str> struct key_type {};

    constexpr static auto fold(auto &consumer) {
        return [&](auto &&...args) { (consumer + ... + args); };
    }

    consteval static auto generator(auto &&...args) {
        return [=](auto &&callable) { return std::invoke(callable, args...); };
    }
};