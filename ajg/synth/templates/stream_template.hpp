//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_STREAM_TEMPLATE_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_STREAM_TEMPLATE_HPP_INCLUDED

#include <istream>
#include <iterator>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/spirit/home/support/multi_pass.hpp>

#include <ajg/synth/templates/base_template.hpp>

namespace ajg {
namespace synth {

//
// bidirectional_input_iterator
//     This horrid creature is needed because of the following reasons:
//         - xpressive requires BidirectionalIterators in order to backtrack
//         - streams only provide InputIterators
//         - spirit's multi_pass only upgrades them to ForwardIterators
////////////////////////////////////////////////////////////////////////////////

template <class Char>
struct bidirectional_input_iterator
    : iterator_facade< bidirectional_input_iterator<Char>
                     , Char
                     , bidirectional_traversal_tag
                     , Char // This forces the 'reference' type to be
                     > {    // a real Value, not an actual reference.
  private:

    typedef Char                                                         char_type;
    typedef std::basic_istream<char_type>                                stream_type;
    typedef boost::spirit::multi_pass<std::istream_iterator<char_type> > iterator_type;

  public:

    typedef typename iterator_type::difference_type difference_type;

  public:

    bidirectional_input_iterator() : iterator_(), stream_() {}

    bidirectional_input_iterator(stream_type& stream)
        : iterator_(std::istream_iterator<Char>(stream)), stream_(stream) {}

    bidirectional_input_iterator(iterator_type const& iterator, stream_type& stream)
        : iterator_(iterator), stream_(stream) {}
  // private:

    bool equal(bidirectional_input_iterator const& that) const { return iterator_ == that.iterator_; }
    void increment() { ++iterator_; }
    void decrement() { throw_exception(not_implemented("decrement")); } // { --iterator; }
    char_type dereference() const { return *iterator_; }
    difference_type distance_to(bidirectional_input_iterator const& that) const { return std::distance(this->iterator_, that.iterator_); }
    bidirectional_input_iterator advance(difference_type const distance) { std::advance(this->iterator_, distance); return *this; }

  private:

    friend class iterator_core_access;
    template <class> friend struct bidirectional_input_iterator;
    iterator_type iterator_;
    boost::optional<stream_type&> stream_;
};

template < class Char
         , class Engine
         , class Stream = std::basic_istream<Char>
         , class Iterator = bidirectional_input_iterator<Char>
         >
struct stream_template
    : public base_template<Engine, Iterator> {

  public:

    typedef Stream   stream_type;
    typedef Iterator iterator_type;

  private:

    typedef base_template<Engine, Iterator> base_type;

  public:

    stream_template(stream_type& stream)
        : base_type(iterator_type(stream), iterator_type()) {}

  /*
  public:

    stream_type const& stream() const { return stream_; }
  */
};

template < class Char
         , class Engine
         >
struct stream_template_identity : boost::mpl::identity<stream_template<Char, Engine> > {};

}} // namespace ajg::synth

#endif // AJG_SYNTH_TEMPLATES_STREAM_TEMPLATE_HPP_INCLUDED
