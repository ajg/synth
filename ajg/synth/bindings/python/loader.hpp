//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_LOADER_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_LOADER_HPP_INCLUDED

#include <ajg/synth/bindings/python/library.hpp>
#include <ajg/synth/bindings/python/conversions.hpp>

namespace ajg {
namespace synth {
namespace bindings {
namespace python {

//
// loader
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Options>
struct loader : Options::abstract_loader {
    typedef Options                                                             options_type;
    typedef typename options_type::library_type                                 library_type;
    typedef typename options_type::traits_type                                  traits_type;

    typedef typename traits_type::string_type                                   string_type;

    explicit loader(py::object const& object) : object_(object) {}
    virtual ~loader() {}

    virtual library_type load_library(string_type const& name) {
        return library_type(new library<options_type>(object_(name)));
    }


  private:

    py::object /*const*/ object_;
};

}}}} // namespace ajg::synth::bindings::python

#endif // AJG_SYNTH_BINDINGS_PYTHON_LOADER_HPP_INCLUDED
