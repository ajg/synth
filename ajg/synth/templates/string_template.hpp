//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_STRING_TEMPLATE_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_STRING_TEMPLATE_HPP_INCLUDED

#include <ajg/synth/templates/base_template.hpp>

namespace ajg {
namespace synth {
namespace templates {

template <class Engine>
struct string_template : base_template<Engine, typename Engine::traits_type::string_type::const_iterator> {
  public:

    typedef string_template                                                     template_type;
    typedef Engine                                                              engine_type;
    typedef typename engine_type::options_type                                  options_type;
    typedef typename options_type::traits_type                                  traits_type;
    typedef typename traits_type::string_type                                   string_type;

  public:

    string_template(string_type const& string) : string_(string) {
        this->reset(this->string_.begin(), this->string_.end());
    }

    string_template(string_type const& string, options_type const& options) : string_(string) {
        this->reset(this->string_.begin(), this->string_.end(), options);
    }

    template <class I>
    string_template(I const& begin, I const& end) : string_(begin, end) {
        this->reset(this->string_.begin(), this->string_.end());
    }

    template <class I>
    string_template(I const& begin, I const& end, options_type const& options) : string_(begin, end) {
        this->reset(this->string_.begin(), this->string_.end(), options);
    }

  public:

    string_type const& string() const { return this->string_; }

  private:

    string_type const string_;
};

}}} // namespace ajg::synth::templates

#endif // AJG_SYNTH_TEMPLATES_STRING_TEMPLATE_HPP_INCLUDED
