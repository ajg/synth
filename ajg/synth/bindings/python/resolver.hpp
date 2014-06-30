//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_RESOLVER_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_RESOLVER_HPP_INCLUDED

#include <ajg/synth/bindings/python/adapter.hpp>
#include <ajg/synth/bindings/python/conversions.hpp>

namespace ajg {
namespace synth {
namespace bindings {
namespace python {

//
// resolver
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Options>
struct resolver : Options::abstract_resolver {
  public:

    typedef Options                                                             options_type;

    typedef typename options_type::value_type                                   value_type;
    typedef typename options_type::context_type                                 context_type;
    typedef typename options_type::traits_type                                  traits_type;

    typedef typename value_type::arguments_type                                 arguments_type;

    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::url_type                                      url_type;

  private:

    typedef conversions<value_type>                                             c;

  public:

    virtual url_type resolve( string_type  const& path
                            , context_type const& context
                            , options_type const& options
                            ) {
        try {
            // TODO: Pass the context and have django-synth handle eliding it.
            return url_type(c::make_string(this->resolver_(path)));
        }
        catch (...) { // TODO: Catch only Resolver404?
            return url_type();
        }
    }


    virtual url_type reverse( string_type    const& name
                            , arguments_type const& arguments
                            , context_type   const& context
                            , options_type   const& options
                            ) {
        try {
            std::pair<py::tuple, py::dict> const args = c::make_args(arguments);
            // TODO: Pass the context instead and let django-synth handle the mapping.
            return url_type(c::make_string(this->reverser_(name, c::make_none(), // == urlconf
                args.first, args.second, context.application())));
        }
        catch (...) { // TODO: Catch only NoReverseMatch?
            return url_type();
        }
    }

    explicit resolver(py::object const& obj)
        : resolver_(obj.attr("resolve"))
        , reverser_(obj.attr("reverse")) {}
    virtual ~resolver() {}

  private:

    py::object /*const*/ resolver_;
    py::object /*const*/ reverser_;
};

}}}} // namespace ajg::synth::bindings::python

#endif // AJG_SYNTH_BINDINGS_PYTHON_RESOLVER_HPP_INCLUDED
