//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_LIBRARY_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_LIBRARY_HPP_INCLUDED

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
struct library : Options::abstract_library_type {
    typedef Options                                                             options_type;
    typedef typename options_type::value_type                                   value_type;
    typedef typename options_type::renderer_type                                renderer_type;
    typedef typename options_type::arguments_type                               arguments_type;
    typedef typename options_type::tag_type                                     tag_type;
    typedef typename options_type::filter_type                                  filter_type;
    typedef typename options_type::context_type                                 context_type;
    typedef typename options_type::tags_type                                    tags_type;
    typedef typename options_type::filters_type                                 filters_type;
    typedef typename options_type::segment_type                                 segment_type;
    typedef typename options_type::segments_type                                segments_type;
    typedef typename options_type::traits_type                                  traits_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::names_type                                    names_type;
    typedef typename traits_type::symbols_type                                  symbols_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

    explicit library(py::object const& lib) {
        if (py::dict tags = py::extract<py::dict>(lib.attr("tags"))) {
            py::stl_input_iterator<string_type> begin(tags.keys()), end;
            tag_names_ = names_type(begin, end);

            BOOST_FOREACH(string_type const& name, tag_names_) {
                py::tuple t(tags[name]);
                py::stl_input_iterator<string_type> begin(t[1]), end;
                this->tags_[name] = tag_type(boost::bind(call_tag, t[0], _1), symbols_type(begin, end));
            }
        }

        if (py::dict filters = py::extract<py::dict>(lib.attr("filters"))) {
            py::stl_input_iterator<string_type> begin(filters.keys()), end;
            filter_names_ = names_type(begin, end);

            BOOST_FOREACH(string_type const& name, filter_names_) {
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

    static string_type call_native_renderer( renderer_type const& renderer
                                           , context_type&        context
                                           , intptr_t             data
                                           ) {
        std::basic_ostringstream<char_type> ss;
        BOOST_ASSERT(!renderer.empty());
        renderer(arguments_type(), ss, context, reinterpret_cast<void const*>(data));
        BOOST_ASSERT(ss);
        return ss.str();
    }

    static void call_python_renderer( py::object     const& r
                                    , arguments_type const& arguments
                                    , ostream_type&         ostream
                                    , context_type&         context
                                    , void const*           data
                                    ) {
        // std::pair<py::tuple, py::dict> const args = from_arguments_with_object(py::object(data), arguments);
        std::pair<py::tuple, py::dict> const args = from_arguments_with_object(py::long_(reinterpret_cast<intptr_t>(data)), arguments);
        ostream << get_string<traits_type>(r(*args.first, **args.second/*TODO: context*/));
    }

    static renderer_type call_tag( py::object    const& tag
                                 , segments_type const& segments
                                 ) {

        typedef boost::mpl::vector<string_type, intptr_t> signature_type;
        py::list l;

        static context_type dummy_context;

        BOOST_FOREACH(segment_type const& segment, segments) {
            boost::function<string_type(intptr_t)> f(boost::bind(call_native_renderer, segment.second
                // TODO: Use the context that the renderer is called with.
                // , boost::ref(context)
                // , _1
                , boost::ref(dummy_context)
                , _1
            ));

            py::list ms;
            BOOST_FOREACH(string_type const& match, segment.first) {
                ms.append(match);
            }
            py::object o = py::make_function(f, py::default_call_policies(), signature_type());

            l.append(py::make_tuple(ms, o));
        }

        return boost::bind(call_python_renderer, tag(l), _1, _2, _3, _4);
    }

    static value_type call_filter( py::object            filter
                                 , value_type     const& value
                                 , arguments_type const& arguments
                                 , context_type&
                                 ) {
        std::pair<py::tuple, py::dict> const args = from_arguments_with(value, arguments);
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
