//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_NONE_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_NONE_HPP_INCLUDED

#include <boost/none_t.hpp>

#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for boost::none_t
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Traits>
struct adapter<Traits, boost::none_t>
    : public abstract_adapter<Traits> {

    AJG_SYNTH_ADAPTER(boost::none_t)
    boost::none_t const& adapted_;

  public:

    number_type  count() const { return 0; }
    boolean_type test()  const { return false; }

 // void input (istream_type& in)        { in  >> adapted_; }
    void output(ostream_type& out) const { out << adapted_; }
};

}} // namespace ajg::synth


namespace std {

template <>
struct less<boost::none_t> {
    bool operator()(boost::none_t const&, boost::none_t const&) const {
        return false;
    }
};

} // namespace std

/*
namespace boost {

inline bool operator <(none_t const&, none_t const&) { return false; }

} // namespace boost
*/

#endif // AJG_SYNTH_ADAPTERS_NONE_HPP_INCLUDED

