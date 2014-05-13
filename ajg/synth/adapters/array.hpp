//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_ARRAY_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_ARRAY_HPP_INCLUDED

namespace boost {

// Forward declaration.
template <class T, std::size_t N> class array;

}

#include <ajg/synth/adapters/concrete_adapter.hpp>
#include <ajg/synth/adapters/container_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for native arrays of statically known size
////////////////////////////////////////////////////////////////////////////////////////////////////

// NOTE: A reference is stored and not a copy because initializing an array within a constructor's
//       initializer list ranges from difficult to impossible in C++03.
// TODO[c++11]: Store array by value.

template <class Behavior, class T, std::size_t N>
struct adapter<Behavior, T[N]> : concrete_adapter<Behavior, T const (&)[N], adapter<Behavior, T[N]> > {
    adapter(T const (&adapted)[N]) : concrete_adapter<Behavior, T const (&)[N], adapter<Behavior, T[N]> >(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Behavior);

    floating_type to_floating() const { return N; }
    boolean_type  to_boolean()  const { return N != 0; }
    range_type    to_range()    const {
        return range_type( static_cast<T const*>(this->adapted())
                         , static_cast<T const*>(this->adapted()) + N
                         );
    }

    void output(ostream_type& out) const { behavior_type::delimited(out, this->to_range()); }
};

//
// specialization for native arrays of statically unknown size
//     TODO[c++14]: Remove support for this nastiness and defer to std::dynarray.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, T[]> : concrete_adapter<Behavior, T* const, adapter<Behavior, T[]> > {
    adapter(T* const adapted, std::size_t const length) : concrete_adapter<Behavior, T* const, adapter<Behavior, T[]> >(adapted), length_(length) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Behavior);

    std::type_info const& type() const { return typeid(T*); } // XXX: return typeid(T[]);

    floating_type to_floating() const { return this->length_; }
    boolean_type  to_boolean()  const { return this->length_ != 0; }
    range_type    to_range()    const {
        return range_type( static_cast<T const*>(this->adapted())
                         , static_cast<T const*>(this->adapted()) + this->length_
                         );
    }

    void output(ostream_type& out) const { behavior_type::delimited(out, this->to_range()); }

  protected:

    virtual boolean_type equal_adapted(adapter_type const& that) const {
        return this->template equal_as<adapter>(that);
    }

    virtual boolean_type less_adapted(adapter_type const& that) const {
        return this->template less_as<adapter>(that);
    }

  private:

    std::size_t length_;
};

//
// specialization for boost::array
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T, std::size_t N>
struct adapter<Behavior, boost::array<T, N> >  : container_adapter<Behavior, boost::array<T, N> > {
    adapter(boost::array<T, N> const& adapted) : container_adapter<Behavior, boost::array<T, N> >(adapted) {}
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_ARRAY_HPP_INCLUDED

