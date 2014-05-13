//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_CONCRETE_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_CONCRETE_ADAPTER_HPP_INCLUDED

#include <boost/type_traits/remove_reference.hpp>

#include <ajg/synth/adapters/adapter.hpp>
#include <ajg/synth/adapters/base_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

template <class Behavior, class Adapted/*, class Holder = Adapted*/, class Specialized = adapter<Behavior, Adapted> >
struct concrete_adapter : base_adapter<Behavior> {
    concrete_adapter(typename boost::remove_reference<Adapted>::type const& adapted) : adapted_(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Behavior);
    typedef typename boost::remove_reference<Adapted>::type bare_adapted_type;

  public:

    virtual std::type_info const& type() const { return typeid(Adapted); }

    virtual string_type to_string() const {
        std::basic_ostringstream<char_type> ostream;
        this->output(ostream);
        return ostream.str();
    }

  protected:

    virtual boolean_type equal_adapted(adapter_type const& that) const { return this->template equal_as<Specialized>(that); }
    virtual boolean_type less_adapted(adapter_type const& that) const { return this->template less_as<Specialized>(that); }

  protected:

    inline bare_adapted_type&       adapted()       { return adapted_; }
    inline bare_adapted_type const& adapted() const { return adapted_; }

  private:

    Adapted adapted_;

  private:

    friend struct base_adapter<behavior_type>;
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_CONCRETE_ADAPTER_HPP_INCLUDED

