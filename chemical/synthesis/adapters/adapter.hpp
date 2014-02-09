
//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ADAPTERS_ADAPTER_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ADAPTERS_ADAPTER_HPP_INCLUDED

#include <boost/lexical_cast.hpp>
#include <boost/type_traits/is_base_of.hpp>

#include <chemical/synthesis/adapters/abstract.hpp>



namespace chemical {
namespace synthesis {

//
// Shortcut macros
////////////////////////////////////////////////////////////////////////////////

#define CHEMICAL_SYNTHESIS_ADAPTER_TYPEDEFS(adaptedT, thisT)    \
  public:                                                    \
                                                             \
    typedef thisT                    this_type;              \
    typedef adaptedT                 adapted_type;           \
    typedef Traits                   traits_type;            \
    typedef abstract_adapter<Traits> abstract_type;          \
                                                             \
    typedef typename traits_type::char_type    char_type;    \
    typedef typename traits_type::size_type    size_type;    \
    typedef typename traits_type::range_type   range_type;   \
    typedef typename traits_type::value_type   value_type;   \
    typedef typename traits_type::boolean_type boolean_type; \
    typedef typename traits_type::number_type  number_type;  \
    typedef typename traits_type::string_type  string_type;  \
    typedef typename traits_type::istream_type istream_type; \
    typedef typename traits_type::ostream_type ostream_type; \
                                                             \
    typedef typename traits_type::iterator       iterator;   \
    typedef typename traits_type::const_iterator const_iterator

#define CHEMICAL_SYNTHESIS_ADAPTER(adaptedT)                    \
    CHEMICAL_SYNTHESIS_ADAPTER_TYPEDEFS(adaptedT, adapter);     \
  public:                                                       \
                                                                \
    adapter(adapted_type const& adapted) : adapted_(adapted) {} \
    std::type_info const& type() const { return typeid(adaptedT); }

//
// unspecialized adapter
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class Adapted>
struct adapter;

#if 0

 : public abstract_adapter<Char> {

    CHEMICAL_SYNTHESIS_ADAPTER_TYPEDEFS(Adapted, adapter);

  public:

    adapter(adapted_type const& adapted) : adapted_(adapted) {}

  public:

    boolean_type test() const { return test_<Adapted>(); }
    void output(ostream_type& out) const { return output_<Adapted>(out); }
    boolean_type equal(abstract_type const& that) const { return equal_<Adapted>(that); }
    std::type_info const& type() const { return type_<Adapted>(); }

    const_iterator begin() const { return begin_<Adapted>(); }
    const_iterator end()   const { return end_  <Adapted>(); }

  private:

    typedef basic_value<Char> base_value;

    template <class T>
    typename enable_if_c<is_base_of<base_value, T>::value, const_iterator>::type begin_() const {
        return adapted_.begin();
    }

    template <class T>
    typename enable_if_c<is_const_iterable<T>::value
                     && !is_base_of<base_value, T>::value, const_iterator>::type begin_() const {
        return adapted_.begin();
    }

    template <class T>
    typename disable_if_c<is_const_iterable<T>::value, const_iterator>::type begin_() const {
        return abstract_type::begin();
    }

    template <class T>
    typename enable_if_c<is_base_of<base_value, T>::value, const_iterator>::type end_() const {
        return adapted_.end();
    }

    template <class T>
    typename enable_if_c<is_const_iterable<T>::value
                     && !is_base_of<base_value, T>::value, const_iterator>::type end_() const {
        return adapted_.end();
    }

    template <class T>
    typename disable_if_c<is_const_iterable<T>::value, const_iterator>::type end_() const {
        return abstract_type::end();
    }

    /*template <class T>
    typename enable_if_c<is_const_iterable<T>::value, boolean_type>::type equal_(... const) const {
        const_iterator const b = this->begin(), e = this->end();
        std::size_t const distance = std::distance(b, e);

        if (!distance e == b || ) {
            return true;
        }
        else if (e - b
    }

    template <class T>
    typename disable_if_c<is_const_iterable<T>::value, boolean_type>::type equal_() const {
        return abstract_type::equal();
    }*/


    template <class T>
    typename enable_if_c<is_base_of<base_value, T>::value, boolean_type>::type test_() const {
        return adapted_;
    }

    template <class T>
    typename enable_if_c<is_const_iterable<T>::value
                     && !is_base_of<base_value, T>::value, boolean_type>::type test_() const {
        return adapted_.empty();
    }

    template <class T>
    typename disable_if_c<is_const_iterable<T>::value, boolean_type>::type test_() const {
        return abstract_type::test();
    }

    template <class T>
    typename enable_if_c<is_base_of<base_value, T>::value, boolean_type>::type equal_(abstract_type const& that) const {
        typedef adapter<Char, reference_wrapper<T const> > cref_type;
        return cref_type(cref(adapted_)).equal(that);
    }

    template <class T>
    typename enable_if_c<is_const_iterable<T>::value
                     && !is_base_of<base_value, T>::value, boolean_type>::type equal_(abstract_type const& that) const {
        return this->compare_sequence(that);
    }

    template <class T>
    typename disable_if_c<is_const_iterable<T>::value, boolean_type>::type equal_(abstract_type const& that) const {
        return abstract_type::equal_(that);
    }

    template <class T>
    typename enable_if_c<is_base_of<base_value, T>::value>::type output_(ostream_type& out) const {
        out << adapted_;
    }

    template <class T>
    typename enable_if_c<is_const_iterable<T>::value
                     && !is_base_of<base_value, T>::value>::type output_(ostream_type& out) const {
        this->list(out);
    }

    template <class T>
    typename disable_if_c<is_const_iterable<T>::value>::type output_(ostream_type& out) const {
        return abstract_type::output(out);
    }

    template <class T>
    typename enable_if<is_base_of<base_value, T>, std::type_info const&>::type type_() const {
        return adapted_.type();
    }

    template <class T>
    typename disable_if<is_base_of<base_value, T>, std::type_info const&>::type type_() const {
        return typeid(T);
    }

    adapted_type const adapted_;

    /*typename mpl::template if_<
        is_convertible<const adapted_type, adapted_type>,
            adapted_type,
            adapted_type&
        >::type const
    adapted_;*/
};



typedef struct { char ar[1]; } no_t;
typedef struct { char ar[4]; } yes_t;

 // has_const_iterator

template <typename T>
no_t has_const_iterator_function( ... );

template <typename T>
yes_t has_const_iterator_function(typename T::const_iterator const volatile *);

template <typename T>
struct has_const_iterator {
     typedef T test_type;

 private:
     static T  t;

 public:

     BOOST_STATIC_CONSTANT(bool, value = sizeof(has_const_iterator_function<T>(0)) == sizeof(yes_t));
};


 // has_element_type

template <typename T>
no_t has_element_type_function( ... );

template <typename T>
yes_t has_element_type_function(typename T::element_type const volatile *);

template <typename T>
struct has_element_type {
     typedef T test_type;

 private:
     static T  t;

 public:

     BOOST_STATIC_CONSTANT(bool, value = sizeof(has_element_type_function<T>(0)) == sizeof(yes_t));
};






template <typename T>
struct is_const_iterable {

    BOOST_STATIC_CONSTANT(bool, value = has_const_iterator<T>::value);

};

template <typename T>
struct is_element_holder {

    BOOST_STATIC_CONSTANT(bool, value = has_element_type<T>::value);

};





#endif

/*
//
// an abstract forwarding type to allow comparisons
////////////////////////////////////////////////////////////////////////////////

template <class Traits, ...>
struct abstract_forwarding_adapter : public abstract_adapter<Traits> {};

*/

template <class Traits, class T, class Adapted, class Derived = adapter<Traits, Adapted> >
struct forwarding_adapter : public abstract_adapter<Traits> {

    CHEMICAL_SYNTHESIS_ADAPTER_TYPEDEFS(Adapted, forwarding_adapter);

  protected:

    forwarding_adapter() {}
    ~forwarding_adapter() {}

  public:

    const_iterator begin() const { return valid() ? forward().begin() : const_iterator(); }
    const_iterator end()   const { return valid() ? forward().end()   : const_iterator(); }

    number_type  count() const { return valid() ? forward().count() : number_type(); }
    boolean_type test()  const { return valid() ? forward().test()  : boolean_type(); }

    boolean_type equal(abstract_type const& that) const {
        return valid() ? forward().equal(that) : boolean_type();
    }

    void input (istream_type& in)        { if (valid()) forward().input(in); }
    void output(ostream_type& out) const { if (valid()) forward().output(out); }

    std::type_info const& type() const { return forward().type(); }

  private:

    typedef typename boost::remove_reference<T>::type T_noref;
    typedef adapter<Traits, reference_wrapper<T> >        ref_type;
    typedef adapter<Traits, reference_wrapper<T_noref const> > cref_type;

  private:

    inline cref_type forward() const {
        return static_cast<Derived const*>
            (this)->template forward<cref_type>();
    }

    // inline T&       get()         { return static_cast<Derived const*>(this)->get(); }
    // inline T const& get()   const { return static_cast<Derived const*>(this)->get(); }
    inline bool     valid() const { return static_cast<Derived const*>(this)->valid(); }

};


//
// Helper function make_adapter
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T>
adapter<Traits, T> make_adapter(T const& t) {
    return adapter<Traits, T>(t);
}

/*
template <class Char, class Value>
struct value_facade;

*/

template <class Traits>
struct adapter<Traits, abstract_adapter<Traits> >; // undefined

//template <class Traits, class Adapted>
//struct adapter<Traits, adapter<Adapted> >; // undefined
/*
//
// specialization for value_facade
////////////////////////////////////////////////////////////////////////////////


template <class Traits, class Char, class Value>
struct adapter<Traits, value_facade<Char, Value> >
    : public forwarding_adapter<Traits, value_facade<Char, Value>, value_facade<Char, Value> > {

    adapter(value_facade<Char, Value> const& adapted) : adapted_(adapted) {}
    value_facade<Char, Value> const adapted_;

    template <class A> A forward() const { return A(ref(adapted_)); }
    bool valid() const { return !adapted_.empty(); }
};
*/

}} // namespace chemical::synthesis

#endif // CHEMICAL_SYNTHESIS_ADAPTERS_ADAPTER_HPP_INCLUDED

