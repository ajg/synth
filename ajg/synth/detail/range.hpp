//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_RANGE_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_RANGE_HPP_INCLUDED

#include <utility>

namespace ajg {
namespace synth {
namespace detail {

//
// pair_range
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class I>
struct pair_range : std::pair<I, I> {
    explicit pair_range() : std::pair<I, I>() {}
    explicit pair_range(I const& b, I const& e) : std::pair<I, I>(b, e) {}
    inline I const& begin() const { return this->first; }
    inline I const& end()   const { return this->second; }
};

//
// make_range
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class I>
inline pair_range<I> make_range(I const& begin, I const& end) {
    return pair_range<I>(begin, end);
}

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_RANGE_HPP_INCLUDED
