
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ENGINES_SSI_VALUE_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ENGINES_SSI_VALUE_HPP_INCLUDED

#include <boost/lexical_cast.hpp>

#include <chemical/synthesis/value_facade.hpp>

namespace chemical {
namespace synthesis {
namespace ssi {

template <class Char>
struct value : value_facade<Char, value<Char> > {
  public:
  
    typedef value                              this_type;
    typedef Char                               char_type;
    typedef value_facade<char_type, this_type> base_type;

  public:

    CHEMICAL_SYNTHESIS_VALUE_CONSTRUCTORS(value, base_type, )

  public:

    inline typename base_type::string_type to_string() const {
        return lexical_cast<typename base_type::string_type>(*this);
    }
};

}}} // namespace chemical::synthesis::ssi

#endif // CHEMICAL_SYNTHESIS_ENGINES_SSI_VALUE_HPP_INCLUDED
