//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_PTIME_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_PTIME_HPP_INCLUDED

#include <boost/date_time/posix_time/posix_time.hpp>

#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for boost::posix_time::ptime
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior>
struct adapter<Behavior, boost::posix_time::ptime>   : concrete_adapter<Behavior, boost::posix_time::ptime> {
    adapter(boost::posix_time::ptime const& adapted) : concrete_adapter<Behavior, boost::posix_time::ptime>(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(boost::posix_time::ptime);

    boolean_type  to_boolean()  const { return !this->adapted_.is_not_a_date_time(); }
    datetime_type to_datetime() const { return traits_type::to_datetime(this->adapted_); }
    void output(ostream_type& out) const { out << this->adapted_; }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_PTREE_HPP_INCLUDED


