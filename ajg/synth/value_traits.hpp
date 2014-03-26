//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_VALUE_TRAITS_HPP_INCLUDED
#define AJG_SYNTH_VALUE_TRAITS_HPP_INCLUDED

#include <map>
#include <vector>
#include <string>
#include <istream>
#include <ostream>
#include <sstream>
#include <utility>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <ajg/synth/value_iterator.hpp>

namespace ajg {
namespace synth {

//
// default_value_traits
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Char, class Value>
struct default_value_traits {
  public:

    typedef Char                                        char_type;
    typedef std::size_t                                 size_type;
    typedef Value                                       value_type;
    typedef bool                                        boolean_type;
    typedef double                                      number_type;
    typedef boost::posix_time::ptime                    datetime_type;
    typedef boost::posix_time::time_duration            duration_type;
    typedef std::basic_string<char_type>                string_type;
    typedef std::basic_istream<char_type>               istream_type;
    typedef std::basic_ostream<char_type>               ostream_type;

    typedef std::vector<value_type>                     sequence_type;
    typedef std::map<string_type, value_type>           mapping_type;

    typedef value_iterator<value_type const>            iterator;
    typedef value_iterator<value_type const>            const_iterator;

    typedef std::pair<const_iterator, const_iterator>   range_type;

  public:

    template <class A, class B>
    inline static B convert(A const& a) {
        B b;
        // Very crude conversion method for now:
        std::basic_stringstream<char_type> stream;
        stream << a;
        stream >> b;
        return b;
    }

    /// transcode
    ///     This function allows us to centralize string conversion
    ///     in order to properly, yet orthogonally, support Unicode.
    ////////////////////////////////////////////////////////////////////////////
    template <class C, class S>
    inline static std::basic_string<C> transcode(S const& string) {
        return std::basic_string<C>(string.begin(), string.end());
    }

/*
    template <class S>
    inline string_type transcode(S const& s) const {
        return boost::lexical_cast<string_type>(s);
    }

    template <class S>
    inline std::string narrow(S const& s) const {
        //return boost::lexical_cast<std::string>(s);
        return std::string(s.begin(), s.end());
    }

    template <class S>
    inline std::wstring widen(S const& s) const {
        //return boost::lexical_cast<std::wstring>(s);
        return std::wstring(s.begin(), s.end());
    }*/
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_VALUE_TRAITS_HPP_INCLUDED
