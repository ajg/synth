//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_STRING_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_STRING_HPP_INCLUDED

#include <string>

#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for std::basic_string
////////////////////////////////////////////////////////////////////////////////////////////////////

template < class Behavior
         , class StringBehavior
         , class Allocator
         >
struct adapter<Behavior, std::basic_string
        <typename Behavior::char_type, StringBehavior, Allocator> >
    : public base_adapter<Behavior> {

    typedef std::basic_string< typename Behavior::char_type
                             , StringBehavior
                             , Allocator
                             >  basic_string_type;
    AJG_SYNTH_ADAPTER(basic_string_type);

  public:

    floating_type to_floating()  const { return behavior_type::to_floating(adapted_); }
    boolean_type to_boolean() const { return !adapted_.empty(); }
    void input (istream_type& in)        { in  >> adapted_; }
    void output(ostream_type& out) const { out << adapted_; }

    const_iterator begin() const { return const_iterator(adapted_.begin()); }
    const_iterator end()   const { return const_iterator(adapted_.end()); }

    /*const*/ adapted_type/*&*/ adapted_;
};

//
// specializations for [const] char_type* and [const] char_type[N].
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior>
struct adapter<Behavior, typename Behavior::char_type const*>
    : public adapter<Behavior, std::basic_string<typename Behavior::char_type> > {
    adapter(typename Behavior::char_type const *const value) :
        adapter<Behavior, std::basic_string<typename Behavior::char_type> >(value) {}
};

template <class Behavior, std::size_t N>
struct adapter<Behavior, typename Behavior::char_type const[N]>
    : public adapter<Behavior, std::basic_string<typename Behavior::char_type> > {
    adapter(typename Behavior::char_type const value[N]) :
        adapter<Behavior, std::basic_string<typename Behavior::char_type> >(
            std::basic_string<typename Behavior::char_type>(value, N ? N - 1 : 0)) {}
};

template <class Behavior>
struct adapter<Behavior, typename Behavior::char_type*>
    : public adapter<Behavior, std::basic_string<typename Behavior::char_type> > {
    adapter(typename Behavior::char_type *const value) :
        adapter<Behavior, std::basic_string<typename Behavior::char_type> >(value) {}
};

template <class Behavior, std::size_t N>
struct adapter<Behavior, typename Behavior::char_type[N]>
    : public adapter<Behavior, std::basic_string<typename Behavior::char_type> > {
    adapter(typename Behavior::char_type const value[N]) :
        adapter<Behavior, std::basic_string<typename Behavior::char_type> >(
            std::basic_string<typename Behavior::char_type>(value, N ? N - 1 : 0)) {}
};

#if AJG_SYNTH_IS_COMPILER_MSVC

    // MSVC seems to need these more specific specializations,
    // otherwise string literals specialize as char arrays.
    template <class Behavior, std::size_t N>
    struct adapter<Behavior, char const[N]>
        : public adapter<Behavior, std::basic_string<char> > {
        adapter(char const value[N]) :
            adapter<Behavior, std::basic_string<char> >(
                std::basic_string<char>(value, N ? N - 1 : 0)) {}
    };

#ifndef AJG_SYNTH_CONFIG_NO_WCHAR_T

    template <class Behavior, std::size_t N>
    struct adapter<Behavior, wchar_t const[N]>
        : public adapter<Behavior, std::basic_string<wchar_t> > {
        adapter(wchar_t const value[N]) :
            adapter<Behavior, std::basic_string<wchar_t> >(
                std::basic_string<wchar_t>(value, N ? N - 1 : 0)) {}
    };

#endif
#endif

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_STRING_HPP_INCLUDED

