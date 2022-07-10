
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
    template <typename S, const std::string_view &key> struct s_obj {
        using type = S;
        S T::*data;

        constexpr s_obj(S T::*data) : data{data} {}

        static constexpr auto get_key() { return key; }
    };

    struct deserializer {
        T ref{};
        std::stringstream str;
        constexpr deserializer(const std::string &str) : str(str) {}

        friend deserializer &operator+(deserializer &s, auto obj) {
            auto idx = s.str.view().find(obj.get_key());
            if (idx != std::string_view::npos) {
                s.str.seekg(static_cast<typename decltype(s.str)::int_type>(
                    idx + obj.get_key().length() + 1));
                s.str >> s.ref.*obj.data;
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

        friend serializer &operator+(serializer &s, auto obj) {
            s.str << obj.get_key() << ':' << s.ref.*obj.data << '\n';
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

        template <typename U>
        requires std::is_convertible_v<typename U::type, S>
        constexpr friend searcher &operator+(searcher &s, U obj) {
            if (obj.get_key() == s.str) {
                s.res = s.ref.*obj.data;
            }
            return s;
        }

        constexpr friend searcher &operator+(searcher &s,
                                             [[maybe_unused]] auto obj) {
            return s;
        }
    };

    template <typename S> constexpr auto search(std::string_view str) const {
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

        template <typename U>
        requires std::is_convertible_v<S, typename U::type>
        constexpr friend setter &operator+(setter &s, U obj) {
            if (obj.get_key() == s.str) {
                s.ref->*obj.data = s.val;
            }
            return s;
        }
        template <typename U>
        constexpr friend setter &operator+(setter &s, [[maybe_unused]] U obj) {
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
            return std::array{[](auto obj) { return obj.get_key(); }(args)...};
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
    requires requires(U u) { std::string_view{u}; }
    constexpr friend value_setter operator+(key_setter s, U obj) {
        return value_setter{s.ref, obj};
    }
    template <typename U>
    constexpr friend value_setter operator+(key_setter s, U obj) = delete;
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
        return T::impl([=, this](auto... args) {
            using obj_type =
                typename get_data_recursive<str, decltype(args)...>::type;
            static_assert(!std::is_void_v<obj_type>);
            using type = typename obj_type::type;

            return static_cast<const T *>(this)->template search<type>(str);
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

    constexpr static auto fold(auto &consumer) {
        return [&](auto &&...args) { (consumer + ... + args); };
    }

    consteval static auto generator(auto &&...args) {
        return [=](auto &&callable) { return std::invoke(callable, args...); };
    }

    template <const std::string_view &key, typename S>
    consteval static auto make_record(S T::*data) {
        return s_obj<S, key>{data};
    }
};