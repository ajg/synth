//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_ARRAY_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_ARRAY_HPP_INCLUDED

namespace boost {

// Forward declaration.
template <class T, std::size_t N> class array;

}

#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for native arrays of statically known size
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T, std::size_t N>
struct adapter<Behavior, T[N]>
    : public base_adapter<Behavior> {

    typedef T array_type[N];
    AJG_SYNTH_ADAPTER(array_type)

  public:

    floating_type to_floating()  const { return N; }
    boolean_type to_boolean() const { return N != 0; }
    void output(ostream_type& out) const { behavior_type::enumerate(*this, out); }
    boolean_type equal(base_type const& that) const { return this->equal_sequence(that); }

    const_iterator begin() const { return const_iterator(pointer<0>()); }
    const_iterator end()   const { return const_iterator(pointer<N>()); }

  private:

    template <std::size_t At>
    inline const T* pointer() const { return adapted_ + At; }

    // We store a reference and not a copy because:
    // a) To store a copy we'd need to initialize it element by element.
    // b) It is almost impossible to envision a situation where there'd be
    //    a temporary native array being created and destroyed prematurely.
    //    UPDATE: Actually, it is not so difficult.
    //      TODO: Consider making this a copy after all.
    // c) The Boost.Array specialization makes use of that fact.
    adapted_type const& adapted_;
};

//
// specialization for native arrays of statically unknown size
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, T[]> : public base_adapter<Behavior> {

    AJG_SYNTH_ADAPTER_TYPEDEFS(T const*);
    // /*const*/ adapted_type adapted_;
    adapted_type adapted_;
    std::size_t const length_;

  protected:

    virtual boolean_type equal_adapted(base_type const& that) const {
        return this->template equal_as<adapter>(that);
    }

    virtual boolean_type less_adapted(base_type const& that) const {
        return this->template less_as<adapter>(that);
    }

  public:

    adapter(adapted_type adapted, std::size_t const length)
        : adapted_(adapted), length_(length) {}

  public:

    floating_type to_floating()  const { return length_; }
    boolean_type to_boolean() const { return length_ != 0; }
    void output(ostream_type& out) const { behavior_type::enumerate(*this, out); }
    boolean_type equal(base_type const& that) const { return this->equal_sequence(that); }

    const_iterator begin() const { return adapted_ + 0; }
    const_iterator end()   const { return adapted_ + length_; }
    std::type_info const& type() const { return typeid(T*); } // XXX: return typeid(T[]);
};

//
// specialization for boost::array
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T, std::size_t N>
struct adapter<Behavior, boost::array<T, N> > : public adapter<Behavior, T[N]> {
    typedef T                  array_type[N];
    typedef boost::array<T, N> adapted_type;

    adapter(adapted_type const& adapted)
        : adapter<Behavior, array_type>(*reinterpret_cast<const array_type*>(adapted.data())) {}
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_ARRAY_HPP_INCLUDED

