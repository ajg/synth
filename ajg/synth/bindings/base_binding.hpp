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

#include <ajg/synth/engines/null/engine.hpp>
#include <ajg/synth/adapters.hpp>
#include <ajg/synth/exceptions.hpp>
#include <ajg/synth/detail/text.hpp>

namespace ajg {
namespace synth {
namespace bindings {

// TODO[c++11]: Make variadic.
template < class Traits
         , template <class E> class Template
         , template <class T> class Django = engines::null::engine
         , template <class T> class SSI    = engines::null::engine
         , template <class T> class TMPL   = engines::null::engine
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
        std::string const name = text::narrow(engine);

        if (name == boost::mpl::c_str<typename django_engine_type::name>::value) {
            this->django_template_ = boost::in_place(source, options);
        }
        else if (name == boost::mpl::c_str<typename ssi_engine_type::name>::value) {
            this->ssi_template_ = boost::in_place(source, options);
        }
        else if (name == boost::mpl::c_str<typename tmpl_engine_type::name>::value) {
            this->tmpl_template_ = boost::in_place(source, options);
        }
        else {
            AJG_SYNTH_THROW(std::invalid_argument("engine: " + name));
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
