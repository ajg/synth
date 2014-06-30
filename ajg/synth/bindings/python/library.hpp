//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_LIBRARY_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_LIBRARY_HPP_INCLUDED

#include <ajg/synth/support.hpp>

#include <memory>
#include <vector>
#include <utility>
#include <stdexcept>

#include <ajg/synth/bindings/python/conversions.hpp>

namespace ajg {
namespace synth {
namespace bindings {
namespace python {

//
// library
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Options>
struct library : Options::abstract_library {
  public:

    typedef Options                                                             options_type;
    typedef typename options_type::value_type                                   value_type;
    typedef typename options_type::renderer_type                                renderer_type;
    typedef typename options_type::tag_type                                     tag_type;
    typedef typename options_type::filter_type                                  filter_type;
    typedef typename options_type::context_type                                 context_type;
    typedef typename options_type::tags_type                                    tags_type;
    typedef typename options_type::filters_type                                 filters_type;
    typedef typename options_type::segment_type                                 segment_type;
    typedef typename options_type::segments_type                                segments_type;

    typedef typename value_type::traits_type                                    traits_type;
    typedef typename value_type::arguments_type                                 arguments_type;

    typedef typename context_type::data_type                                    data_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::timezone_type                                 timezone_type;
    typedef typename traits_type::paths_type                                    paths_type;
    typedef typename traits_type::names_type                                    names_type;
    typedef typename traits_type::symbols_type                                  symbols_type;
    typedef typename traits_type::language_type                                 language_type;
    typedef typename traits_type::formats_type                                  formats_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

  private:

    typedef detail::text<string_type>                                           text;
    typedef conversions<value_type>                                             c;

  public:


    explicit library(py::object const& lib) {
        if (py::dict tags = py::extract<py::dict>(lib.attr("tags"))) {
            this->tag_names_ = c::make_names(tags.keys());

            BOOST_FOREACH(string_type const& name, this->tag_names_) {
                py::tuple t(tags[name]);
                py::object const& fn       = t[0];
                py::object const& mns      = t[1];
                py::object const& lns      = t[2];
                py::object const& simple   = t[3];
                py::object const& dataless = t[4];

                if (!fn) {
                    AJG_SYNTH_THROW(std::invalid_argument("no tag function"));
                }
                tag_type tag;
                tag.simple   = c::make_boolean(simple);
                tag.dataless = c::make_boolean(dataless);
                tag.function = boost::bind(call_tag, fn, tag.simple, tag.dataless, _1);
                if (mns) tag.middle_names = c::make_symbols(mns);
                if (lns) tag.last_names   = c::make_symbols(lns);

                this->tags_[name] = tag;
            }
        }

        if (py::dict const& filters = py::extract<py::dict>(lib.attr("filters"))) {
            this->filter_names_ = c::make_names(filters.keys());

            BOOST_FOREACH(string_type const& name, this->filter_names_) {
                this->filters_[name] = filter_type(boost::bind(call_filter, filters[name], _1, _2, _3));
            }
        }
    }
    virtual ~library() {}

    virtual boolean_type has_tag(string_type const& name) const { return this->tags_.find(name) != this->tags_.end(); }
    virtual boolean_type has_filter(string_type const& name) const { return this->filters_.find(name) != this->filters_.end(); }
    virtual names_type   list_tags() const { return this->tag_names_; }
    virtual names_type   list_filters() const { return this->filter_names_; }
    virtual tag_type     get_tag(string_type const& name) { return this->tags_[name]; }
    virtual filter_type  get_filter(string_type const& name) { return this->filters_[name]; }

    inline static context_type& local_context(context_type* const c = 0) {
        static AJG_SYNTH_THREAD_LOCAL context_type* context = 0;
        return c ? *(context = c) : *context;
    }

    typedef boost::mpl::vector
            < string_type
            , py::object
            , boolean_type
            , boolean_type
            , py::object
            , py::object
            , py::object
            , py::object> python_renderer_signature_type;

    static string_type call_native_renderer( renderer_type const& renderer
                                           , py::object    const& data
                                           , boolean_type  const  caseless    = false
                                           , boolean_type  const  safe        = false
                                           , py::object    const  application = py::object()
                                           , py::object    const  timezone    = py::object()
                                           , py::object    const  language    = py::object()
                                           , py::object    const  formats     = py::object()
                                           ) {
        context_type& context = local_context();
        std::basic_ostringstream<char_type> ss;
        ss.imbue(traits_type::standard_locale());
        BOOST_ASSERT(!renderer.empty());

        // TODO: Use a stage to push and pop all these values.
        context.caseless(caseless);
        context.safe(safe);
        context.application(application ? c::make_string(application) : string_type());
        timezone_type const old_tz = context.timezone(c::make_timezone(timezone));
        context.language(c::make_language(language));
        if (formats) {
            context.formats(c::make_formats(py::extract<py::dict>(formats)));
        }

        renderer(arguments_type(), ss, context);
        BOOST_ASSERT(ss);
        context.timezone(old_tz);
        return ss.str();
    }

    static void call_python_renderer( py::object     const& renderer
                                    , boolean_type   const  simple
                                    , boolean_type   const  dataless
                                    , arguments_type        arguments
                                    , ostream_type&         ostream
                                    , context_type&         context
                                    ) {
        local_context(&context);

        if (!dataless) {
            arguments.first.insert(arguments.first.begin(), 1, context.data());
        }

        if (!simple) {
            arguments.second[text::literal("caseless")]    = value_type(context.caseless());
            arguments.second[text::literal("safe")]        = value_type(context.safe());
            arguments.second[text::literal("application")] = value_type(context.application());
            arguments.second[text::literal("timezone")]    = value_type(context.timezone());
            arguments.second[text::literal("language")]    = value_type(context.language());
            arguments.second[text::literal("formats")]     = value_type(formats_type()); // value_type(context.formats());
        }

        std::pair<py::tuple, py::dict> const args = c::make_args(arguments);
        ostream << c::make_string(renderer(*args.first, **args.second));
    }

    static renderer_type call_tag( py::object    const& tag
                                 , boolean_type  const  simple
                                 , boolean_type  const  dataless
                                 , segments_type const& segments
                                 ) {
        if (simple) {
            return boost::bind(call_python_renderer, tag(), simple, dataless, _1, _2, _3);
        }

        py::list segs;

        BOOST_FOREACH(segment_type const& segment, segments) {
            // boost::function<typename boost::function_types::function_type<python_renderer_signature_type>::type>
            //     f(boost::bind(call_native_renderer, segment.second, _1, _2, _3, _4, _5, _6, _7));

            py::list pcs;
            BOOST_FOREACH(string_type const& piece, segment.first) {
                pcs.append(piece);
            }
            py::object const rndr = py::make_function(
                boost::bind(call_native_renderer, segment.second, _1, _2, _3, _4, _5, _6, _7),
                py::default_call_policies(),
                    ( py::arg("context")
                    , py::arg("caseless")    = false
                    , py::arg("safe")        = false
                    , py::arg("application") = py::object()
                    , py::arg("timezone")    = py::object()
                    , py::arg("language")    = py::object()
                    , py::arg("formats")     = py::object()
                    ), python_renderer_signature_type());

            segs.append(py::make_tuple(pcs, rndr));
        }

        return boost::bind(call_python_renderer, tag(segs), simple, dataless, _1, _2, _3);
    }

    static value_type call_filter( py::object            filter
                                 , value_type     const& value
                                 , arguments_type const& rest
                                 , context_type&
                                 ) {
        // TODO: Pass context and metadata.

        std::pair<py::tuple, py::dict> const args = c::make_args_with(value, rest);
        return filter(*args.first, **args.second);
    }

  private:

    names_type tag_names_;
    names_type filter_names_;

    tags_type    tags_;
    filters_type filters_;
};

}}}} // namespace ajg::synth::bindings::python

#endif // AJG_SYNTH_BINDINGS_PYTHON_LIBRARY_HPP_INCLUDED
