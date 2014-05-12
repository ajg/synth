//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_CONCRETE_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_CONCRETE_ADAPTER_HPP_INCLUDED

#include <ajg/synth/adapters/adapter.hpp>
#include <ajg/synth/adapters/base_adapter.hpp>

namespace ajg {
namespace synth {

template <class Behavior, class Adapted/*, class Holder = Adapted*/, class Sub = adapter<Behavior, Adapted> >
struct concrete_adapter : base_adapter<Behavior> {
    concrete_adapter(Adapted const& adapted) : adapted_(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Adapted);

  public:

    std::type_info const& type() const { return typeid(Adapted); }

  protected:

    virtual boolean_type equal_adapted(adapter_type const& that) const { return this->template equal_as<Sub/*concrete_adapter*/>(that); }
    virtual boolean_type less_adapted(adapter_type const& that) const { return this->template less_as<Sub/*concrete_adapter*/>(that); }

  protected:

    adapted_type adapted_; // TODO: Expose an accessor adapted() instead.

  private:

    friend struct base_adapter<behavior_type>;
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_CONCRETE_ADAPTER_HPP_INCLUDED

