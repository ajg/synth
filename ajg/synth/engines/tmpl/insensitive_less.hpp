//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_TMPL_INSENSITIVE_LESS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_TMPL_INSENSITIVE_LESS_HPP_INCLUDED

#include <locale>
#include <algorithm>
#include <functional>

namespace ajg {
namespace synth {
namespace engines {
namespace tmpl {

//
// insensitive_less:
//     Case-insensitive version of std::less<T>.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
struct insensitive_less : std::binary_function<T, T, bool> {
  public:

    typedef typename T::value_type char_type;

  public:

    bool operator ()(T const& t1, T const& t2) const {
        return std::lexicographical_compare
            ( t1.begin(), t1.end()
            , t2.begin(), t2.end()
            , comparer()
            );
    }

  private:

    struct comparer : std::binary_function<char_type, char_type, bool> {
        bool operator ()(char_type const a, char_type const b) const {
            return (std::tolower)(a) < (std::tolower)(b);
        }
    };
};

}}}} // namespace ajg::synth::engines::tmpl

#endif // AJG_SYNTH_ENGINES_TMPL_INSENSITIVE_LESS_HPP_INCLUDED
