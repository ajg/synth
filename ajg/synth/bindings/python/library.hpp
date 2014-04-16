//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_LIBRARY_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_LIBRARY_HPP_INCLUDED

#include <memory>
#include <vector>
#include <utility>
#include <stdexcept>

#include <ajg/synth/bindings/python/detail.hpp>

namespace ajg {
namespace synth {
namespace python {

//
// library
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Options>
struct library : Options::abstract_library_type {
    typedef Options                                      options_type;
    typedef typename options_type::boolean_type          boolean_type;
    typedef typename options_type::string_type           string_type;
    typedef typename options_type::value_type            value_type;
    typedef typename options_type::arguments_type        arguments_type;
    typedef typename options_type::names_type            names_type;
    typedef typename options_type::tag_type              tag_type;
    typedef typename options_type::filter_type           filter_type;
    typedef typename options_type::context_type          context_type;
    typedef typename options_type::tags_type             tags_type;
    typedef typename options_type::filters_type          filters_type;

    explicit library(py::object const& lib) {
        if (py::dict tags = py::extract<py::dict>(lib.attr("tags"))) {
            py::stl_input_iterator<string_type> begin(tags.keys()), end;
            tag_names_ = names_type(begin, end);

            BOOST_FOREACH(string_type const& name, tag_names_) {
                tags_[name] = tag_type(boost::bind(call_tag, tags[name], _1, _2, _3));
            }
        }

        if (py::dict filters = py::extract<py::dict>(lib.attr("filters"))) {
            py::stl_input_iterator<string_type> begin(filters.keys()), end;
            filter_names_ = names_type(begin, end);

            BOOST_FOREACH(string_type const& name, filter_names_) {
                filters_[name] = filter_type(boost::bind(call_filter, filters[name], _1, _2, _3, _4));
            }
        }
    }
    virtual ~library() {}

    virtual boolean_type has_tag(string_type const& name) const { return tags_.find(name) != tags_.end(); }
    virtual boolean_type has_filter(string_type const& name) const { return filters_.find(name) != filters_.end(); }
    virtual names_type   list_tags() const { return tag_names_; }
    virtual names_type   list_filters() const { return filter_names_; }
    virtual tag_type     get_tag(string_type const& name) { return tags_[name]; }
    virtual filter_type  get_filter(string_type const& name) { return filters_[name]; }

    static value_type call_tag(py::object tag, options_type&, context_type*, arguments_type& arguments) {
        std::pair<py::tuple, py::dict> const args = d::from_arguments(arguments);
        return tag(*args.first, **args.second);
    }

    static value_type call_filter( py::object            filter
                                 , options_type   const&
                                 , context_type   const&
                                 , value_type     const& value
                                 , arguments_type const& arguments
                                 ) {
        std::pair<py::tuple, py::dict> const args = d::from_arguments(value, arguments);
        return filter(*args.first, **args.second);
    }

  private:

    names_type /*const*/ tag_names_;
    names_type /*const*/ filter_names_;

    tags_type    /*const*/ tags_;
    filters_type /*const*/ filters_;
};

}}} // namespace ajg::synth::python

#endif // AJG_SYNTH_BINDINGS_PYTHON_LIBRARY_HPP_INCLUDED
