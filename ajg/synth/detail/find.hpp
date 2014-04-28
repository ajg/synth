//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_FIND_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_FIND_HPP_INCLUDED

#include <algorithm>

#include <boost/optional.hpp>

namespace ajg {
namespace synth {
namespace detail {

//
// has_mapped_type
////////////////////////////////////////////////////////////////////////////////////////////////////

struct one { char c[1]; };
struct two { char c[2]; };

template <class T> one has_mapped_type_(...);
template <class T> two has_mapped_type_(typename T::mapped_type const volatile *);

template <class T> struct has_mapped_type {
    BOOST_STATIC_CONSTANT(bool, value = sizeof(has_mapped_type_<T>(0)) == sizeof(two));
};

//
// find:
//     Uniform interface for mapped and non-mapped containers.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Needle, class Container>
inline boost::optional<typename Container::value_type> find
        ( Needle    const& needle
        , Container const& container
        , typename boost::disable_if<has_mapped_type<Container> >::type* = 0
        ) {
    typename Container::const_iterator const it = std::find(container.begin(), container.end(), needle);
    if (it == container.end()) return boost::none; else return *it;
}

template <class Container>
inline boost::optional<typename Container::mapped_type> find
        ( typename Container::key_type const& needle
        , Container                    const& container
        ) {
    typename Container::const_iterator const it = container.find(needle);
    if (it == container.end()) return boost::none; else return it->second;
}

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_FIND_HPP_INCLUDED
