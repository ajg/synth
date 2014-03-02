//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_DETAIL_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_DETAIL_HPP_INCLUDED

#include <iterator>

#include <ajg/synth/config.hpp>

#include <boost/optional.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/spirit/home/support/multi_pass.hpp>

namespace ajg {
namespace synth {
namespace detail {

using boost::optional;
using boost::throw_exception;

//
// bidirectional_istream_iterator
//     This horrid creature is needed because of the following reasons:
//         - istreams only provide InputIterators
//         - spirit's multi_pass can only upgrade them to ForwardIterators
//         - xpressive requires BidirectionalIterators in order to backtrack
////////////////////////////////////////////////////////////////////////////////

template <class Stream, class Char = typename Stream::char_type>
struct bidirectional_istream_iterator
    : boost::iterator_facade< bidirectional_istream_iterator<Stream, Char>
                            , Char
                            , boost::bidirectional_traversal_tag
                            , Char // This forces the 'reference' type to be
                            > {    // a real Value, not an actual reference.
  private:

    typedef Char                                           char_type;
    typedef Stream                                         stream_type;
    typedef std::istream_iterator<char_type>               input_iterator_type;
    typedef boost::spirit::multi_pass<input_iterator_type> forward_iterator_type;

  public:

    typedef typename forward_iterator_type::difference_type difference_type;

  public:

    bidirectional_istream_iterator() : forward_iterator_(), stream_() {}

    bidirectional_istream_iterator(stream_type& stream)
        : forward_iterator_(std::istream_iterator<Char>(stream))
        , stream_(stream) {}

    bidirectional_istream_iterator( forward_iterator_type const& forward_iterator
                                  , stream_type&                 stream
                                  )
        : forward_iterator_(forward_iterator)
        , stream_(stream) {}

  // private:

    bool equal(bidirectional_istream_iterator const& that) const {
        return forward_iterator_ == that.forward_iterator_;
    }

    void increment() { ++forward_iterator_; }
    void decrement() { throw_exception(not_implemented("decrement")); } // { --iterator; }

    char_type dereference() const { return *forward_iterator_; }

    difference_type distance_to(bidirectional_istream_iterator const& that) const {
        return std::distance(this->forward_iterator_, that.forward_iterator_);
    }

    bidirectional_istream_iterator advance(difference_type const distance) {
        std::advance(this->forward_iterator_, distance);
        return *this;
    }

  private:

    friend class iterator_core_access;
    template <class S, class C> friend struct bidirectional_istream_iterator;

    forward_iterator_type         forward_iterator_;
    boost::optional<stream_type&> stream_;
};

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_TEMPLATES_DETAIL_HPP_INCLUDED
