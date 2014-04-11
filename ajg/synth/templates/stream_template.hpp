//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_STREAM_TEMPLATE_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_STREAM_TEMPLATE_HPP_INCLUDED

#include <istream>

#include <boost/mpl/identity.hpp>
#include <boost/utility/base_from_member.hpp>

#include <ajg/synth/templates/detail.hpp>
#include <ajg/synth/templates/base_template.hpp>

namespace ajg {
namespace synth {

template < class Char
         , class Engine
         , class Stream   = std::basic_istream<Char>
         , class Iterator = typename detail::bidirectional_input_stream<Stream>::iterator
         >
struct stream_template
    : private boost::base_from_member<detail::bidirectional_input_stream<Stream> >
    , public base_template<Engine, Iterator> {

  public:

    typedef Stream   stream_type;
    typedef Iterator iterator_type;

  private:

    typedef detail::bidirectional_input_stream<stream_type>                     input_stream_type;
    typedef boost::base_from_member<input_stream_type>                          base_member_type;
    typedef base_template<Engine, iterator_type>                                base_type;

  public:

    stream_template(stream_type& stream)
        : base_member_type(input_stream_type(stream))
        , base_type(base_member_type::member.begin(), base_member_type::member.end()) {}
};

template < class Char
         , class Engine
         >
struct stream_template_identity : boost::mpl::identity<stream_template<Char, Engine> > {};

}} // namespace ajg::synth

#endif // AJG_SYNTH_TEMPLATES_STREAM_TEMPLATE_HPP_INCLUDED
