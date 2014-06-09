//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_PTIME_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_PTIME_HPP_INCLUDED

#include <boost/date_time/posix_time/posix_time.hpp>

#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for boost::posix_time::ptime
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct adapter<Value, boost::posix_time::ptime>      : concrete_adapter<Value, boost::posix_time::ptime, chronologic> {
    adapter(boost::posix_time::ptime const& adapted) : concrete_adapter<Value, boost::posix_time::ptime, chronologic>(adapted) {}

    virtual optional<typename Value::boolean_type>  get_boolean()  const { return !this->adapted().is_not_a_date_time(); }
    virtual optional<typename Value::datetime_type> get_datetime() const { return Value::traits_type::to_datetime(this->adapted()); }
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_PTREE_HPP_INCLUDED


