//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_DROP_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_DROP_HPP_INCLUDED

#include <utility>

#include <ajg/synth/detail/advance_to.hpp>

namespace ajg {
namespace synth {
namespace detail {

//
// drop
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Container, class Number>
inline std::pair<typename Container::const_iterator, typename Container::const_iterator>
        drop(Container const& container, Number const number) {
    return std::pair<typename Container::const_iterator, typename Container::const_iterator>(
        advance_to(container.begin(), number), container.end());
}

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_DROP_HPP_INCLUDED
