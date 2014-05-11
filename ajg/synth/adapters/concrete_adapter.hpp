//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_CONCRETE_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_CONCRETE_ADAPTER_HPP_INCLUDED

#include <ajg/synth/adapters/adapter.hpp>
#include <ajg/synth/adapters/base_adapter.hpp>

namespace ajg {
namespace synth {

template <class Behavior, class Adapted>
struct concrete_adapter : base_adapter<Behavior> {
  public:

    AJG_SYNTH_ADAPTER_TYPEDEFS(Adapted);

    friend struct base_adapter<behavior_type>;

  protected:

    concrete_adapter(adapted_type const& adapted) : adapted_(adapted) {}

  public:

    std::type_info const& type() const { return typeid(adapted_type); }

  protected:

    virtual boolean_type equal_adapted(base_type const& that) const { return this->template equal_as<concrete_adapter>(that); }
    virtual boolean_type less_adapted(base_type const& that) const { return this->template less_as<concrete_adapter>(that); }

  protected:

    adapted_type adapted_;
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_CONCRETE_ADAPTER_HPP_INCLUDED

