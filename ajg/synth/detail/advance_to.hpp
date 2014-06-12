//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_ADVANCE_TO_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_ADVANCE_TO_HPP_INCLUDED

#include <iterator>

namespace ajg {
namespace synth {
namespace detail {

//
// advance_to:
//     Simulates operator + for iterators which lack it.
//     NOTE: Doesn't do any bounds checking; ensure distance is valid.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Iterator, class Distance>
inline Iterator advance_to(Iterator iterator, Distance const distance) {
    std::advance(iterator, distance);
    return iterator;
}

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_ADVANCE_TO_HPP_INCLUDED
