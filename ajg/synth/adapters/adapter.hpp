//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_ADAPTER_HPP_INCLUDED

#include <boost/type_traits/is_base_of.hpp>

#include <ajg/synth/adapters/abstract.hpp>

namespace ajg {
namespace synth {

using namespace boost; // FIXME: Delete.

//
// Shortcut macros
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_SYNTH_ADAPTER_TYPEDEFS(adaptedT, thisT)            \
  public:                                                      \
                                                               \
    typedef thisT                    this_type;                \
    typedef adaptedT                 adapted_type;             \
    typedef Traits                   traits_type;              \
    typedef abstract_adapter<Traits> abstract_type;            \
    friend struct abstract_adapter<Traits>;                    \
                                                               \
    typedef typename traits_type::char_type     char_type;     \
    typedef typename traits_type::size_type     size_type;     \
    typedef typename traits_type::range_type    range_type;    \
    typedef typename traits_type::value_type    value_type;    \
    typedef typename traits_type::boolean_type  boolean_type;  \
    typedef typename traits_type::number_type   number_type;   \
    typedef typename traits_type::string_type   string_type;   \
    typedef typename traits_type::datetime_type datetime_type; \
    typedef typename traits_type::duration_type duration_type; \
    typedef typename traits_type::istream_type  istream_type;  \
    typedef typename traits_type::ostream_type  ostream_type;  \
                                                               \
    typedef typename traits_type::iterator       iterator;     \
    typedef typename traits_type::const_iterator const_iterator


// TODO: Refactor this into a concrete_adapter<T>.
#define AJG_SYNTH_ADAPTER(adaptedT) \
    AJG_SYNTH_ADAPTER_TYPEDEFS(adaptedT, adapter); \
  protected: \
    virtual boolean_type equal_adapted(abstract_type const& that) const { return this->template equal_as<adapter>(that); } \
    virtual boolean_type less_adapted(abstract_type const& that) const { return this->template less_as<adapter>(that); } \
  public: \
    adapter(adapted_type const& adapted) : adapted_(adapted) {} \
    std::type_info const& type() const { return typeid(adaptedT); }

//
// adapter (unspecialized)
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Traits, class Adapted>
struct adapter;

// TODO: Move to separate file.
template <class Traits, class T, class Adapted, class Derived = adapter<Traits, Adapted> >
struct forwarding_adapter : public abstract_adapter<Traits> {

    AJG_SYNTH_ADAPTER_TYPEDEFS(Adapted, forwarding_adapter);

  protected:

    forwarding_adapter() {}
    ~forwarding_adapter() {}

  protected:

    virtual boolean_type equal_adapted(abstract_type const& that) const {
        return forward().template equal_as<adapter<Traits, T> >(that);
    }

    virtual boolean_type less_adapted(abstract_type const& that) const {
        return forward().template less_as<adapter<Traits, T> >(that);
    }

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

    typedef typename boost::remove_reference<T>::type               T_noref;
    typedef adapter<Traits, reference_wrapper<T> >                  ref_type;
    typedef adapter<Traits, reference_wrapper<T_noref const> >      cref_type;

  private:

    inline cref_type forward() const {
        return static_cast<Derived const*>(this)->template forward<cref_type>();
    }

    // inline T&       get()         { return static_cast<Derived const*>(this)->get(); }
    // inline T const& get()   const { return static_cast<Derived const*>(this)->get(); }
    inline bool     valid() const { return static_cast<Derived const*>(this)->valid(); }

};

//
// make_adapter
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T>
adapter<Traits, T> make_adapter(T const& t) {
    return adapter<Traits, T>(t);
}

template <class Traits>
struct adapter<Traits, abstract_adapter<Traits> >; // undefined

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_ADAPTER_HPP_INCLUDED

