//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_MULTI_TEMPLATE_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_MULTI_TEMPLATE_HPP_INCLUDED

#include <memory>
#include <vector>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

namespace ajg {
namespace synth {
namespace detail {

// TODO: Replace with multi_engine.
template <class Char, class Django, class SSI, class TMPL>
struct multi_template {

  public:

    typedef Char   char_type;
    typedef Django django_engine_type;
    typedef SSI    ssi_engine_type;
    typedef TMPL   tmpl_engine_type;

    typedef string_template<char_type, django_engine_type> django_template_type;
    typedef string_template<char_type, ssi_engine_type>    ssi_template_type;
    typedef string_template<char_type, tmpl_engine_type>   tmpl_template_type;

    typedef bool                      boolean_type;
    typedef std::basic_string<Char>   string_type;
    typedef std::vector<string_type>  directories_type;

    // typedef typename engine_type::context_type context_type;

  public:

    multi_template( string_type      const& source
                  , string_type      const& engine_name
                  , boolean_type     const  autoescape
                  , string_type      const& default_value
                  , directories_type const& directories
                  )
        : django_template_(engine_name == "django" ? new django_template_type(source) : 0)
        , ssi_template_   (engine_name == "ssi"    ? new ssi_template_type   (source) : 0)
        , tmpl_template_  (engine_name == "tmpl"   ? new tmpl_template_type  (source) : 0)
        , django_options_(autoescape, default_value, directories) {

        if (!django_template_ && !ssi_template_ && !tmpl_template_) {
            throw std::invalid_argument("engine_name");
        }
    }

  protected:

    template <class X, class Y>
    string_type render_to_string(Y const& y) const {
        if (django_template_) {
            return django_template_->render_to_string(
                X::template get_context<typename django_template_type::context_type>(y), django_options_);
        }
        else if (ssi_template_) {
            return ssi_template_->render_to_string(
                X::template get_context<typename ssi_template_type::context_type>(y)); // TODO: ssi_options_.
        }
        else if (tmpl_template_) {
            return tmpl_template_->render_to_string(
                X::template get_context<typename tmpl_template_type::context_type>(y)); // TODO: tmpl_options_.
        }
        AJG_UNREACHABLE;
    }

  private:

    // TODO: Use unique_ptr?
    boost::shared_ptr<django_template_type>     django_template_;
    boost::shared_ptr<ssi_template_type>        ssi_template_;
    boost::shared_ptr<tmpl_template_type>       tmpl_template_;
    typename django_template_type::options_type django_options_;
};

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_TEMPLATES_MULTI_TEMPLATE_HPP_INCLUDED
