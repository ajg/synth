//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_STRING_TEMPLATE_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_STRING_TEMPLATE_HPP_INCLUDED

#include <string>

#include <boost/utility/base_from_member.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <ajg/synth/templates/base_template.hpp>

namespace ajg {
namespace synth {

template < class Char
         , class Engine
         , class String = std::basic_string<Char>
         , class Iterator = typename String::const_iterator
         >
struct string_template
    : private base_from_member<String>
    , public base_template<Engine, Iterator> {

  public:

    // Do this in case String happens to be a reference type
    // e.g. to eliminate copies, in which case we want to avoid
    // creating references to references, which are illegal.
    typedef typename remove_reference<String>::type string_type;

  private:

    typedef base_template<Engine, Iterator> base_type;

  public:

    string_template(string_type const& string)
        : base_from_member<String>(string)
        , base_type(this->member.begin(), this->member.end()) {}

    template <class Iterator_>
    string_template(Iterator_ const& begin, Iterator_ const& end)
        : base_from_member<String>(string_type(begin, end))
        , base_type(this->member.begin(), this->member.end()) {}

  public:

    string_type const& str() const { return this->member(); }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_TEMPLATES_STRING_TEMPLATE_HPP_INCLUDED
