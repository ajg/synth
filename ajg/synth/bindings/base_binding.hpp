//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_BASE_BINDING_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_BASE_BINDING_HPP_INCLUDED

#include <ajg/synth/support.hpp>

#include <memory>
#include <vector>
#include <stdexcept>

#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/utility/in_place_factory.hpp>

#include <ajg/synth/engines.hpp>
#include <ajg/synth/adapters.hpp>
#include <ajg/synth/exceptions.hpp>
#include <ajg/synth/detail/text.hpp>

namespace ajg {
namespace synth {
namespace bindings {

namespace django = ::ajg::synth::engines::django;
namespace ssi    = ::ajg::synth::engines::ssi;
namespace tmpl   = ::ajg::synth::engines::tmpl;

template < class Traits
         , template <class E> class Template
         , template <class T> class Django = django::engine
         , template <class T> class SSI    = ssi::engine
         , template <class T> class TMPL   = tmpl::engine
         >
struct base_binding : boost::noncopyable {
  public:

    typedef base_binding                                                        base_binding_type;
    typedef Traits                                                              traits_type;

    typedef engines::base_engine<traits_type>                                   base_engine_type;
    typedef Django<traits_type>                                                 django_engine_type;
    typedef SSI<traits_type>                                                    ssi_engine_type;
    typedef TMPL<traits_type>                                                   tmpl_engine_type;

    typedef Template<django_engine_type>                                        django_template_type;
    typedef Template<ssi_engine_type>                                           ssi_template_type;
    typedef Template<tmpl_engine_type>                                          tmpl_template_type;

    typedef typename base_engine_type::context_type                             context_type;
    typedef typename base_engine_type::options_type                             options_type;

/*
    typedef django_options_type                                                 options_type;
    typedef typename django_options_type::arguments_type                        arguments_type;
    typedef typename django_options_type::formats_type                          formats_type;
 // typedef typename django_options_type::markers_type                          markers_type;
    typedef typename django_options_type::library_type                          library_type;
    typedef typename django_options_type::libraries_type                        libraries_type;
    typedef typename django_options_type::loader_type                           loader_type;
    typedef typename django_options_type::loaders_type                          loaders_type;
    typedef typename django_options_type::resolver_type                         resolver_type;
    typedef typename django_options_type::resolvers_type                        resolvers_type;
*/

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::ostream_type                                  ostream_type;
    typedef typename traits_type::paths_type                                    paths_type;

  private:

    typedef detail::text<string_type>                                           text;

  public:

    template <class Source>
    base_binding(Source const& source, string_type const& engine, options_type const& options) {
        if (engine == text::literal("django")) {
            this->django_template_ = boost::in_place/*<django_template_type>*/(source, options);
        }
        else if (engine == text::literal("ssi")) {
            this->ssi_template_ = boost::in_place/*<ssi_template_type>*/(source, options);
        }
        else if (engine == text::literal("tmpl")) {
            this->tmpl_template_ = boost::in_place/*<tmpl_template_type>*/(source, options);
        }
        else {
            AJG_SYNTH_THROW(std::invalid_argument("engine"));
        }
    }

  protected:

    void render_to_stream(ostream_type& ostream, context_type& context) const {
             if (django_template_) return django_template_->render_to_stream(ostream, context);
        else if (ssi_template_)    return ssi_template_->render_to_stream(ostream, context);
        else if (tmpl_template_)   return tmpl_template_->render_to_stream(ostream, context);
        else AJG_SYNTH_THROW(std::logic_error("missing template"));
    }

    string_type render_to_string(context_type& context) const {
             if (django_template_) return django_template_->render_to_string(context);
        else if (ssi_template_)    return ssi_template_->render_to_string(context);
        else if (tmpl_template_)   return tmpl_template_->render_to_string(context);
        else AJG_SYNTH_THROW(std::logic_error("missing template"));
    }

    void render_to_path(string_type const& path, context_type& context) const {
             if (django_template_) return django_template_->render_to_path(path, context);
        else if (ssi_template_)    return ssi_template_->render_to_path(path, context);
        else if (tmpl_template_)   return tmpl_template_->render_to_path(path, context);
        else AJG_SYNTH_THROW(std::logic_error("missing template"));
    }

  private:

    // TODO: Consider using a variant instead.
    boost::optional<django_template_type> django_template_;
    boost::optional<ssi_template_type>    ssi_template_;
    boost::optional<tmpl_template_type>   tmpl_template_;
};

}}} // namespace ajg::synth::bindings

#endif // AJG_SYNTH_BINDINGS_BASE_BINDING_HPP_INCLUDED
