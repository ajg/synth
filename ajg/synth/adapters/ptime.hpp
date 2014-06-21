//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_PTIME_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_PTIME_HPP_INCLUDED

// TODO: Move local_time adapters to own files and add a date_time group header.
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>

#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for boost::posix_time::ptime
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct adapter<Value, boost::posix_time::ptime>                : concrete_adapter<Value, boost::posix_time::ptime, chronologic> {
    adapter(          boost::posix_time::ptime const& adapted) : concrete_adapter<Value, boost::posix_time::ptime, chronologic>(adapted) {}

    virtual optional<typename Value::boolean_type>  get_boolean()  const { return !this->adapted().is_not_a_date_time(); }
    virtual optional<typename Value::datetime_type> get_datetime() const { return Value::traits_type::to_datetime(this->adapted()); }
};

//
// specialization for boost::local_time::local_date_time
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct adapter<Value, boost::local_time::local_date_time>                : concrete_adapter<Value, boost::local_time::local_date_time, chronologic> {
    adapter(          boost::local_time::local_date_time const& adapted) : concrete_adapter<Value, boost::local_time::local_date_time, chronologic>(adapted) {}

    virtual optional<typename Value::boolean_type>  get_boolean()  const { return !this->adapted().is_not_a_date_time(); }
    virtual optional<typename Value::datetime_type> get_datetime() const { return this->adapted(); }
};

//
// specialization for boost::local_time::time_zone_ptr
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct adapter<Value, boost::local_time::time_zone_ptr>                : concrete_adapter<Value, boost::local_time::time_zone_ptr> {
    adapter(          boost::local_time::time_zone_ptr const& adapted) : concrete_adapter<Value, boost::local_time::time_zone_ptr>(adapted) {}

    virtual optional<typename Value::boolean_type>  get_boolean() const { return this->adapted().get() != 0; }
    virtual optional<typename Value::string_type>   get_string()  const { return this->adapted() ? this->adapted()->to_posix_string() : typename Value::string_type(); }
};

/*
//
// specialization for boost::date_time::time_zone_base
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value, class Char>
struct adapter<Value, boost::date_time::time_zone_base<boost::posix_time::ptime, Char> >               : concrete_adapter<Value, boost::date_time::time_zone_base<boost::posix_time::ptime, Char> > {
    adapter(          boost::date_time::time_zone_base<boost::posix_time::ptime, Char> const& adapted) : concrete_adapter<Value, boost::date_time::time_zone_base<boost::posix_time::ptime, Char> >(adapted) {}
};
*/

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_PTREE_HPP_INCLUDED


