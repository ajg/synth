//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTHESIS_ADAPTERS_VARIANT_HPP_INCLUDED
#define AJG_SYNTHESIS_ADAPTERS_VARIANT_HPP_INCLUDED

#include <boost/variant/variant_fwd.hpp>

#include <ajg/synthesis/adapters/adapter.hpp>

namespace ajg {
namespace synthesis {

//
// specialization for boost::variant
////////////////////////////////////////////////////////////////////////////////

template <class Traits, BOOST_VARIANT_ENUM_PARAMS(class T)>
struct adapter<Traits, variant<BOOST_VARIANT_ENUM_PARAMS(T)> >
    : public abstract_adapter<Traits> {

    typedef variant<BOOST_VARIANT_ENUM_PARAMS(T)> variant_type;
    AJG_SYNTHESIS_ADAPTER(variant_type)
    adapted_type const adapted_;

  public:

    // FIXME: These should be forwarded to the real value.
    boolean_type equal(abstract_type const& that) const { return compare(*this, that); }
    number_type  count() const { return adapted_.which(); }
    boolean_type test()  const { return true; }


    // TODO: These rely on T0 ... TN all having these operators available,
    //       which obviously isn't the general case. So it might be better
    //       to switch to using one of our own individual adapters for
    //       this purpose (probably through a static_visitor.)
    void input (ostream_type& in)        { adapted_ >> in; }
    void output(ostream_type& out) const { out << adapted_; }
};

}} // namespace ajg::synthesis

#endif // AJG_SYNTHESIS_ADAPTERS_OPTIONAL_HPP_INCLUDED

