//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_STRING_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_STRING_HPP_INCLUDED

#include <string>

#include <ajg/synth/adapters/container_adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for std::basic_string
////////////////////////////////////////////////////////////////////////////////////////////////////

template < class Behavior
         , class Traits
         , class Allocator
         >
struct adapter<Behavior, std::basic_string<typename Behavior::char_type, Traits, Allocator> >  : container_adapter<Behavior, std::basic_string<typename Behavior::char_type, Traits, Allocator> > {
    adapter(std::basic_string<typename Behavior::char_type, Traits, Allocator> const& adapted) : container_adapter<Behavior, std::basic_string<typename Behavior::char_type, Traits, Allocator> >(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(std::basic_string<typename Behavior::char_type, Traits, Allocator>);

    floating_type to_floating() const { return behavior_type::to_floating(this->adapted_); }

    void input (istream_type& in)        { in  >> this->adapted_; }
    void output(ostream_type& out) const { out << this->adapted_; }
};

//
// specializations for [const] char_type* and [const] char_type[N].
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior>
struct adapter<Behavior, typename Behavior::char_type const*> : adapter<Behavior, std::basic_string<typename Behavior::char_type> > {
    adapter(typename Behavior::char_type const *const value)  : adapter<Behavior, std::basic_string<typename Behavior::char_type> >(value) {}
};

template <class Behavior, std::size_t N>
struct adapter<Behavior, typename Behavior::char_type const[N]> : adapter<Behavior, std::basic_string<typename Behavior::char_type> > {
    adapter(typename Behavior::char_type const value[N])        : adapter<Behavior, std::basic_string<typename Behavior::char_type> >(
            std::basic_string<typename Behavior::char_type>(value, N ? N - 1 : 0)) {}
};

template <class Behavior>
struct adapter<Behavior, typename Behavior::char_type*> : adapter<Behavior, std::basic_string<typename Behavior::char_type> > {
    adapter(typename Behavior::char_type *const value)  : adapter<Behavior, std::basic_string<typename Behavior::char_type> >(value) {}
};

template <class Behavior, std::size_t N>
struct adapter<Behavior, typename Behavior::char_type[N]> : adapter<Behavior, std::basic_string<typename Behavior::char_type> > {
    adapter(typename Behavior::char_type const value[N])  : adapter<Behavior, std::basic_string<typename Behavior::char_type> >(
            std::basic_string<typename Behavior::char_type>(value, N ? N - 1 : 0)) {}
};

#if AJG_SYNTH_IS_COMPILER_MSVC

    // MSVC seems to need these more specific specializations,
    // otherwise string literals specialize as char arrays.
    template <class Behavior, std::size_t N>
    struct adapter<Behavior, char const[N]> : adapter<Behavior, std::basic_string<char> > {
        adapter(char const value[N]) :
            adapter<Behavior, std::basic_string<char> >(std::basic_string<char>(value, N ? N - 1 : 0)) {}
    };

#ifndef AJG_SYNTH_CONFIG_NO_WCHAR_T

    template <class Behavior, std::size_t N>
    struct adapter<Behavior, wchar_t const[N]> : adapter<Behavior, std::basic_string<wchar_t> > {
        adapter(wchar_t const value[N]) :
            adapter<Behavior, std::basic_string<wchar_t> >(std::basic_string<wchar_t>(value, N ? N - 1 : 0)) {}
    };

#endif
#endif

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_STRING_HPP_INCLUDED

