//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTHESIS_ENGINES_SSI_VALUE_HPP_INCLUDED
#define AJG_SYNTHESIS_ENGINES_SSI_VALUE_HPP_INCLUDED

#include <boost/lexical_cast.hpp>

#include <ajg/synthesis/value_facade.hpp>

namespace ajg {
namespace synthesis {
namespace ssi {

template <class Char>
struct value : value_facade<Char, value<Char> > {
  public:

    typedef value                              this_type;
    typedef Char                               char_type;
    typedef value_facade<char_type, this_type> base_type;

  public:

    AJG_SYNTHESIS_VALUE_CONSTRUCTORS(value, base_type, {})

  public:

    inline typename base_type::string_type to_string() const {
        return lexical_cast<typename base_type::string_type>(*this);
    }
};

}}} // namespace ajg::synthesis::ssi

#endif // AJG_SYNTHESIS_ENGINES_SSI_VALUE_HPP_INCLUDED
