//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_STRING_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_STRING_HPP_INCLUDED

#include <string>

#include <ajg/synth/adapters/container_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for std::basic_string
////////////////////////////////////////////////////////////////////////////////////////////////////

template < class Value
         , class Traits
         , class Allocator
         >
struct adapter<Value, std::basic_string<typename Value::char_type, Traits, Allocator> >     : container_adapter<Value, std::basic_string<typename Value::char_type, Traits, Allocator>, type_flags(textual | sequential)> {
    adapter(std::basic_string<typename Value::char_type, Traits, Allocator> const& adapted) : container_adapter<Value, std::basic_string<typename Value::char_type, Traits, Allocator>, type_flags(textual | sequential)>(adapted) {}
    virtual optional<typename Value::string_type> get_string() const { return this->adapted(); }
};

//
// specializations for [const] char_type* and [const] char_type[N].
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct adapter<Value, typename Value::char_type const*>   : adapter<Value, std::basic_string<typename Value::char_type> > {
    adapter(typename Value::char_type const *const value) : adapter<Value, std::basic_string<typename Value::char_type> >(value) {}
};

template <class Value, std::size_t N>
struct adapter<Value, typename Value::char_type const[N]> : adapter<Value, std::basic_string<typename Value::char_type> > {
    adapter(typename Value::char_type const value[N])     : adapter<Value, std::basic_string<typename Value::char_type> >(
            std::basic_string<typename Value::char_type>(value, N ? N - 1 : 0)) {}
};

template <class Value>
struct adapter<Value, typename Value::char_type*>   : adapter<Value, std::basic_string<typename Value::char_type> > {
    adapter(typename Value::char_type *const value) : adapter<Value, std::basic_string<typename Value::char_type> >(value) {}
};

template <class Value, std::size_t N>
struct adapter<Value, typename Value::char_type[N]>   : adapter<Value, std::basic_string<typename Value::char_type> > {
    adapter(typename Value::char_type const value[N]) : adapter<Value, std::basic_string<typename Value::char_type> >(
            std::basic_string<typename Value::char_type>(value, N ? N - 1 : 0)) {}
};

#if AJG_SYNTH_IS_COMPILER_MSVC

    // MSVC seems to need these more specific specializations,
    // otherwise string literals specialize as char arrays.
    template <class Value, std::size_t N>
    struct adapter<Value, char const[N]> : adapter<Value, std::basic_string<char> > {
        adapter(char const value[N])     : adapter<Value, std::basic_string<char> >(
            std::basic_string<char>(value, N ? N - 1 : 0)) {}
    };

#ifndef AJG_SYNTH_CONFIG_NO_WCHAR_T

    template <class Value, std::size_t N>
    struct adapter<Value, wchar_t const[N]> : adapter<Value, std::basic_string<wchar_t> > {
        adapter(wchar_t const value[N])     : adapter<Value, std::basic_string<wchar_t> >(
            std::basic_string<wchar_t>(value, N ? N - 1 : 0)) {}
    };

#endif
#endif

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_STRING_HPP_INCLUDED

