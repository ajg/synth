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
         , template <class T> class Engine0 = engines::null::engine
         , template <class T> class Engine1 = engines::null::engine
         , template <class T> class Engine2 = engines::null::engine
         , template <class T> class Engine3 = engines::null::engine
         , template <class T> class Engine4 = engines::null::engine
         >
struct base_binding : boost::noncopyable {
  public:

    typedef base_binding                                                        base_binding_type;
    typedef Traits                                                              traits_type;

    typedef engines::base_engine<traits_type>                                   base_engine_type;
    typedef Engine0<traits_type>                                                engine0_type;
    typedef Engine1<traits_type>                                                engine1_type;
    typedef Engine2<traits_type>                                                engine2_type;
    typedef Engine3<traits_type>                                                engine3_type;
    typedef Engine4<traits_type>                                                engine4_type;

    typedef boost::mpl::c_str<typename engine0_type::name>                      name0;
    typedef boost::mpl::c_str<typename engine1_type::name>                      name1;
    typedef boost::mpl::c_str<typename engine2_type::name>                      name2;
    typedef boost::mpl::c_str<typename engine3_type::name>                      name3;
    typedef boost::mpl::c_str<typename engine4_type::name>                      name4;

    // TODO: Consider using a variant instead.
    typedef boost::optional<Template<engine0_type> >                            template0_type;
    typedef boost::optional<Template<engine1_type> >                            template1_type;
    typedef boost::optional<Template<engine2_type> >                            template2_type;
    typedef boost::optional<Template<engine3_type> >                            template3_type;
    typedef boost::optional<Template<engine4_type> >                            template4_type;

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
             if (name == name0::value) this->template0_ = boost::in_place(source, options);
        else if (name == name1::value) this->template1_ = boost::in_place(source, options);
        else if (name == name2::value) this->template2_ = boost::in_place(source, options);
        else if (name == name3::value) this->template3_ = boost::in_place(source, options);
        else if (name == name4::value) this->template4_ = boost::in_place(source, options);
        else AJG_SYNTH_THROW(std::invalid_argument("engine: " + name));
    }

  protected:

    void render_to_stream(ostream_type& ostream, context_type& context) const {
             if (template0_) return template0_->render_to_stream(ostream, context);
        else if (template1_) return template1_->render_to_stream(ostream, context);
        else if (template2_) return template2_->render_to_stream(ostream, context);
        else if (template3_) return template3_->render_to_stream(ostream, context);
        else if (template4_) return template4_->render_to_stream(ostream, context);
        else AJG_SYNTH_THROW(std::logic_error("missing template"));
    }

    string_type render_to_string(context_type& context) const {
             if (template0_) return template0_->render_to_string(context);
        else if (template1_) return template1_->render_to_string(context);
        else if (template2_) return template2_->render_to_string(context);
        else if (template3_) return template3_->render_to_string(context);
        else if (template4_) return template4_->render_to_string(context);
        else AJG_SYNTH_THROW(std::logic_error("missing template"));
    }

    void render_to_path(string_type const& path, context_type& context) const {
             if (template0_) return template0_->render_to_path(path, context);
        else if (template1_) return template1_->render_to_path(path, context);
        else if (template2_) return template2_->render_to_path(path, context);
        else if (template3_) return template3_->render_to_path(path, context);
        else if (template4_) return template4_->render_to_path(path, context);
        else AJG_SYNTH_THROW(std::logic_error("missing template"));
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
