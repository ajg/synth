//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_CONTAINER_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_CONTAINER_HPP_INCLUDED

#include <ajg/synth/support.hpp>

#include <cmath>
#include <string>
#include <utility>
#include <exception>
#include <stdexcept>

#include <boost/assert.hpp>
#include <boost/optional.hpp>
#include <boost/type_traits/make_signed.hpp>

#include <ajg/synth/exceptions.hpp>

namespace ajg {
namespace synth {
namespace detail {

//
// slice:
//     Accepts the indices for a half-open range [lower, upper) and returns said range as a pair of
//     iterators; imitates Python's sequence slicing including negative indices, which are "rotated"
//     into their positive counterparts. The indices are bounds-checked regardless of their sign.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Container>
inline std::pair<typename Container::const_iterator, typename Container::const_iterator> slice
        ( Container                                                                         const& container
        , boost::optional<typename boost::make_signed<typename Container::size_type>::type> const  lower     = boost::none
        , boost::optional<typename boost::make_signed<typename Container::size_type>::type> const  upper     = boost::none
        ) {
    typedef typename Container::size_type                size_type;
    typedef typename boost::make_signed<size_type>::type index_type;

    size_type const size = container.size();
    index_type lower_ = lower.get_value_or(0);
    index_type upper_ = upper.get_value_or(size);

    // Adjust negative indices to the right position.
    if (lower_ < 0) lower_ = static_cast<index_type>(size) + lower_;
    if (upper_ < 0) upper_ = static_cast<index_type>(size) + upper_;

    // Check for indices that are out of range.
    if (lower_ < 0 || static_cast<size_type>(lower_) > size) AJG_SYNTH_THROW(std::out_of_range("lower index"));
    if (upper_ < 0 || static_cast<size_type>(upper_) > size) AJG_SYNTH_THROW(std::out_of_range("upper index"));
    if (lower_ > upper_)                                     AJG_SYNTH_THROW(std::logic_error("reversed indices"));

    // Move to the right places.
    typename Container::const_iterator first  = container.begin();
    typename Container::const_iterator second = first;
    std::advance(first, lower_);
    std::advance(second, upper_);
    return std::make_pair(first, second);
}

//
// at:
//     Accepts an index which can be negative, which is "rotated" into its positive counterpart.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Container>
inline typename Container::const_iterator at
        ( Container                                                        const& container
        , typename boost::make_signed<typename Container::size_type>::type const  index
        ) {
    typedef typename Container::size_type                size_type;
    typedef typename boost::make_signed<size_type>::type index_type;

    size_type  const size   = container.size();
    index_type const index_ = index < 0 ? static_cast<index_type>(size) + index : index;

    if (index_ < 0 || static_cast<size_type>(index_) > size) AJG_SYNTH_THROW(std::out_of_range("index"));

    // TODO: Once we have value_iterator::advance_to consider using return begin() + index,
    //       to be O(1). For now, we must use this O(n) method:
    typename Container::const_iterator       it  = container.begin();
    typename Container::const_iterator const end = container.end();

    for (size_type i = 0; it != end; ++it, ++i) {
        if (i == static_cast<size_type>(index_)) {
            return it;
        }
    }

    AJG_SYNTH_THROW(std::invalid_argument("index"));
}

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

//
// drop
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Container, class Number>
inline std::pair<typename Container::const_iterator, typename Container::const_iterator>
        drop(Container const& container, Number const number) {
    return std::make_pair(advance_to(container.begin(), number), container.end());
}

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_CONTAINER_HPP_INCLUDED
