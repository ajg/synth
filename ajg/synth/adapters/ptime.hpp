//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_PTIME_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_PTIME_HPP_INCLUDED

#include <boost/date_time/posix_time/posix_time.hpp>

#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for boost::posix_time::ptime
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior>
struct adapter<Behavior, boost::posix_time::ptime>
    : public base_adapter<Behavior> {

    typedef boost::posix_time::ptime ptime_type;
    AJG_SYNTH_ADAPTER(ptime_type)
    adapted_type adapted_;

  public:

    boolean_type to_boolean() const { return !adapted_.is_not_a_date_time(); }
    datetime_type to_datetime() const { return adapted_; }
    void output(ostream_type& out) const { out << adapted_; }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_PTREE_HPP_INCLUDED


