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
         , class Foreign
         , template <class E> class Template
         , template <class T> class Engine0
         , template <class T> class Engine1 = engines::null::engine
         , template <class T> class Engine2 = engines::null::engine
         , template <class T> class Engine3 = engines::null::engine
         , template <class T> class Engine4 = engines::null::engine
         >
struct base_binding : boost::noncopyable {
  public:

    typedef base_binding                                                        base_binding_type;
    typedef Traits                                                              traits_type;
    typedef Foreign                                                             foreign_type;

    typedef engines::null::engine<traits_type>                                  null_engine_type;
    typedef Engine0<traits_type>                                                engine0_type;
    typedef Engine1<traits_type>                                                engine1_type;
    typedef Engine2<traits_type>                                                engine2_type;
    typedef Engine3<traits_type>                                                engine3_type;
    typedef Engine4<traits_type>                                                engine4_type;

    // Note: A boost::variant won't work here because it doesn't seem to support in-place construction.
    typedef Template<null_engine_type>                                          null_template_type;
    typedef boost::optional<Template<engine0_type> >                            template0_type;
    typedef boost::optional<Template<engine1_type> >                            template1_type;
    typedef boost::optional<Template<engine2_type> >                            template2_type;
    typedef boost::optional<Template<engine3_type> >                            template3_type;
    typedef boost::optional<Template<engine4_type> >                            template4_type;

    typedef typename null_template_type::source_type                            source_type;
    typedef typename null_engine_type::options_type                             options_type;

    // typedef typename options_type::context_type                              context_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::ostream_type                                  ostream_type;
    typedef typename traits_type::paths_type                                    paths_type;

  private:

    typedef detail::text<string_type>                                           text;

  protected:

    base_binding(source_type source, string_type const& engine, options_type const& options) {
        std::string const name = text::narrow(engine);
             if (name == engine0_type::name()) this->template0_ = boost::in_place(boost::ref(source), options);
        else if (name == engine1_type::name()) this->template1_ = boost::in_place(boost::ref(source), options);
        else if (name == engine2_type::name()) this->template2_ = boost::in_place(boost::ref(source), options);
        else if (name == engine3_type::name()) this->template3_ = boost::in_place(boost::ref(source), options);
        else if (name == engine4_type::name()) this->template4_ = boost::in_place(boost::ref(source), options);
        else AJG_SYNTH_THROW(std::invalid_argument("engine: " + name));
    }

  protected:

    void render_to_stream(ostream_type& ostream, foreign_type& data) const {
             if (template0_) return template0_->render_to_stream(ostream, data);
        else if (template1_) return template1_->render_to_stream(ostream, data);
        else if (template2_) return template2_->render_to_stream(ostream, data);
        else if (template3_) return template3_->render_to_stream(ostream, data);
        else if (template4_) return template4_->render_to_stream(ostream, data);
        else AJG_SYNTH_THROW(std::logic_error("missing template"));
    }

    string_type render_to_string(foreign_type& data) const {
             if (template0_) return template0_->render_to_string(data);
        else if (template1_) return template1_->render_to_string(data);
        else if (template2_) return template2_->render_to_string(data);
        else if (template3_) return template3_->render_to_string(data);
        else if (template4_) return template4_->render_to_string(data);
        else AJG_SYNTH_THROW(std::logic_error("missing template"));
    }

    void render_to_path(string_type const& path, foreign_type& data) const {
             if (template0_) return template0_->render_to_path(path, data);
        else if (template1_) return template1_->render_to_path(path, data);
        else if (template2_) return template2_->render_to_path(path, data);
        else if (template3_) return template3_->render_to_path(path, data);
        else if (template4_) return template4_->render_to_path(path, data);
        else AJG_SYNTH_THROW(std::logic_error("missing template"));
    }

    inline options_type const& options() const {
             if (template0_) return template0_->options();
        else if (template1_) return template1_->options();
        else if (template2_) return template2_->options();
        else if (template3_) return template3_->options();
        else if (template4_) return template4_->options();
        else AJG_SYNTH_THROW(std::logic_error("missing template"));
    }

    inline static options_type& default_options(options_type const* const o = 0) {
        // TODO: Make thread-safe (but not thread-local.)
        static options_type options;
        if (o != 0) options = *o;
        return options;
    }

  private:

    template0_type template0_;
    template1_type template1_;
    template2_type template2_;
    template3_type template3_;
    template4_type template4_;
};

}}} // namespace ajg::synth::bindings

#endif // AJG_SYNTH_BINDINGS_BASE_BINDING_HPP_INCLUDED
