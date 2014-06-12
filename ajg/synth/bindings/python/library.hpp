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

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::names_type                                    names_type;
    typedef typename traits_type::symbols_type                                  symbols_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

  private:

    typedef conversions<value_type>                                             c;

  public:

    explicit library(py::object const& lib) {
        if (py::dict tags = py::extract<py::dict>(lib.attr("tags"))) {
            py::stl_input_iterator<string_type> begin(tags.keys()), end;
            tag_names_ = names_type(begin, end);

            BOOST_FOREACH(string_type const& name, tag_names_) {
                py::tuple t(tags[name]);
                py::object const& fn = t[0];
                py::object const& mn = t[1];
                py::object const& ln = t[2];

                if (!fn) {
                    AJG_SYNTH_THROW(std::invalid_argument("tag function"));
                }
                tag_type tag;
                tag.function = boost::bind(call_tag, fn, _1);
                if (mn) tag.middle_names.insert(py::stl_input_iterator<string_type>(mn), end);
                if (ln) tag.last_names.insert(py::stl_input_iterator<string_type>(ln), end);
                this->tags_[name] = tag;
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
                                        // , context_type&        context
                                           , py::object const&    data
                                           ) {
        context_type& context = local_context();
        // TODO: Make previous/data exception safe.
        value_type const previous = context.data(data);
        std::basic_ostringstream<char_type> ss;
        BOOST_ASSERT(!renderer.empty());
        renderer(arguments_type(), ss, context);
        context.data(previous);
        BOOST_ASSERT(ss);
        return ss.str();
    }

    inline static context_type& local_context(context_type* const c = 0) {
        static AJG_SYNTH_THREAD_LOCAL context_type* context = c;
        BOOST_ASSERT(context);
        return c ? *(context = c) : *context;
    }

    static void call_python_renderer( py::object     const& r
                                    , arguments_type const& rest
                                    , ostream_type&         ostream
                                    , context_type&         context
                                    ) {
        local_context(&context);
        std::pair<py::tuple, py::dict> const args = c::make_args_with(context.data(), rest);
        ostream << c::make_string(r(*args.first, **args.second));
    }

    static renderer_type call_tag( py::object    const& tag
                                 , segments_type const& segments
                                 ) {

        typedef boost::mpl::vector<string_type, py::object> signature_type;
        py::list l;

        BOOST_FOREACH(segment_type const& segment, segments) {
            boost::function<string_type(py::object)> f(boost::bind(call_native_renderer, segment.second, _1));

            py::list ms;
            BOOST_FOREACH(string_type const& match, segment.first) {
                ms.append(match);
            }
            py::object o = py::make_function(f, py::default_call_policies(), signature_type());

            l.append(py::make_tuple(ms, o));
        }

        return boost::bind(call_python_renderer, tag(l), _1, _2, _3);
    }

    static value_type call_filter( py::object            filter
                                 , value_type     const& value
                                 , arguments_type const& rest
                                 , context_type&
                                 ) {
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
