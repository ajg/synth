//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_NUMERIC_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_NUMERIC_ADAPTER_HPP_INCLUDED

#include <cmath>
#include <vector>
#include <iomanip>

#include <boost/cstdint.hpp>
#include <boost/io/ios_state.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>

#include <ajg/synth/adapters/adapter.hpp>
#include <ajg/synth/detail/is_integer.hpp>

namespace ajg {
namespace synth {

//
// numeric_adapter:
//     Base adapter implementation for primitive numeric types
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class Numeric>
struct numeric_adapter : public base_adapter<Behavior> {

    AJG_SYNTH_ADAPTER_TYPEDEFS(Numeric, numeric_adapter);
    adapted_type /*const*/ adapted_;

  protected:

    numeric_adapter(adapted_type const& adapted) : adapted_(adapted) {}

  protected:

    virtual boolean_type equal_adapted(base_type const& that) const {
        return this->template equal_as<numeric_adapter>(that);
    }

    virtual boolean_type less_adapted(base_type const& that) const {
        return this->template less_as<numeric_adapter>(that);
    }

  public:

    std::type_info const& type() const { return typeid(Numeric); }

    boolean_type is_numeric() const { return true; }

    floating_type to_floating() const { return static_cast<floating_type>(adapted_); }
    boolean_type to_boolean() const { return adapted_ != Numeric(0); }

    void input (istream_type& in)        { in  >> adapted_; }
    void output(ostream_type& out) const { output_number<Numeric>(out); }

  private:

    // For floating-point types
    template <class T>
    void output_number(ostream_type& out, typename boost::disable_if<boost::is_integral<T> >::type* = 0) const {
        boost::io::basic_ios_all_saver<char_type> saver(out);

        if (detail::is_integer(adapted_)) {
            out << std::fixed << std::setprecision(0);
        }

        if (out.flags() & out.hex) {
        // If they hex is desired, we convert to an
        // integral first, and then output as hex.
            out << static_cast<integer_type>(adapted_);
        }
        else {
            out << adapted_;
        }
    }

    // For integral types
    template <class T>
    void output_number(ostream_type& out, typename boost::enable_if<boost::is_integral<T> >::type* = 0) const {
        out << adapted_;
    }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_NUMERIC_ADAPTER_HPP_INCLUDED

