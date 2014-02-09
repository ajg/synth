//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTHESIS_ADAPTERS_ARRAY_HPP_INCLUDED
#define AJG_SYNTHESIS_ADAPTERS_ARRAY_HPP_INCLUDED

namespace boost {

// Forward declaration.
template <class T, std::size_t N> class array;

}

#include <ajg/synthesis/adapters/adapter.hpp>

namespace ajg {
namespace synthesis {

//
// specialization for native arrays of statically known size
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T, std::size_t N>
struct adapter<Traits, T[N]>
    : public abstract_adapter<Traits> {

    typedef T array_type[N];
    AJG_SYNTHESIS_ADAPTER(array_type)

  public:

    number_type  count() const { return N; }
    boolean_type test()  const { return N != 0; }
    void output(ostream_type& out) const { this->list(out); }
    boolean_type equal(abstract_type const& that) const {
        return this->compare_sequence(that);
    }

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
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T>
struct adapter<Traits, T[]>
    : public abstract_adapter<Traits> {

    AJG_SYNTHESIS_ADAPTER_TYPEDEFS(T const*, adapter);
    // /*const*/ adapted_type adapted_;
    adapted_type adapted_;
    std::size_t const length_;

  public:

    adapter(adapted_type adapted, std::size_t const length)
        : adapted_(adapted), length_(length) {}

  public:

    number_type  count() const { return length_; }
    boolean_type test()  const { return length_ != 0; }
    void output(ostream_type& out) const { this->list(out); }
    boolean_type equal(abstract_type const& that) const {
        return this->compare_sequence(that);
    }

    const_iterator begin() const { return adapted_ + 0; }
    const_iterator end()   const { return adapted_ + length_; }
    std::type_info const& type() const { return typeid(T[]); }
};

//
// specialization for boost::array
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T, std::size_t N>
struct adapter<Traits, array<T, N> > : public adapter<Traits, T[N]> {
    typedef T array_type[N];
    adapter(array<T, N> const& value) : adapter<Traits, array_type>
        (*reinterpret_cast<const array_type*>(value.data())) {}
};

/*
template <class Traits, class T, std::size_t N>
struct adapter<Traits, array<T, N> >
    : public abstract_adapter<Traits> {

    typedef array<T, N> array_type;
    AJG_SYNTHESIS_ADAPTER(array_type)

  public:

    ...

    adapted_type const adapted_;
};*/

}} // namespace ajg::synthesis

#endif // AJG_SYNTHESIS_ADAPTERS_ARRAY_HPP_INCLUDED

