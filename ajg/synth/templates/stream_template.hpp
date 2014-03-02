//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_STREAM_TEMPLATE_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_STREAM_TEMPLATE_HPP_INCLUDED

#include <istream>

#include <boost/mpl/identity.hpp>

#include <ajg/synth/templates/detail.hpp>
#include <ajg/synth/templates/base_template.hpp>

namespace ajg {
namespace synth {

template < class Char
         , class Engine
         , class Stream   = std::basic_istream<Char>
         , class Iterator = detail::bidirectional_istream_iterator<Stream>
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
