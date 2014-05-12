//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_NUMERIC_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_NUMERIC_ADAPTER_HPP_INCLUDED

#include <iomanip>

#include <boost/io/ios_state.hpp>
#include <boost/type_traits/is_integral.hpp>

#include <ajg/synth/detail/is_integer.hpp>
#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {

//
// numeric_adapter:
//     Base adapter implementation for primitive numeric types
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class Adapted>
struct numeric_adapter : concrete_adapter<Behavior, Adapted> {
    numeric_adapter(Adapted const& adapted) : concrete_adapter<Behavior, Adapted>(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Behavior);

    boolean_type  is_numeric()  const { return true; }
    floating_type to_floating() const { return static_cast<floating_type>(this->adapted()); }
    boolean_type  to_boolean()  const { return this->adapted() != static_cast<Adapted>(0); }

    void input (istream_type& in)        { in >> this->adapted(); }
    void output(ostream_type& out) const {
        // The fast, happy, integral path:
        if (boost::is_integral<Adapted>::value) {
            out << this->adapted();
            return;
        }

        // The slow, sad, fractional path:
        boost::io::basic_ios_all_saver<char_type> saver(out);

        if (detail::is_integer(this->adapted())) {
            out << std::fixed << std::setprecision(0);
        }

        if (out.flags() & out.hex) {
        // If they hex is desired, we convert to an
        // integral first, and then output as hex.
            out << static_cast<integer_type>(this->adapted());
        }
        else {
            out << this->adapted();
        }
    }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_NUMERIC_ADAPTER_HPP_INCLUDED

