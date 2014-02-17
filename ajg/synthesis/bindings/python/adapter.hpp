//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTHESIS_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED
#define AJG_SYNTHESIS_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED


#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>

#include <ajg/synthesis/adapters/adapter.hpp>

namespace ajg {
namespace synthesis {

//
// specialization for boost::python::object
////////////////////////////////////////////////////////////////////////////////

template <class Traits>
struct adapter<Traits, boost::python::object>
    : public abstract_adapter<Traits> {

    typedef boost::python::object object_type;
    AJG_SYNTHESIS_ADAPTER(object_type)
    object_type const adapted_;

  private:

    typedef typename boost::python::stl_input_iterator<value_type> stl_iterator;

  public:

    boolean_type test() const {

        /*if (!adapted_) {
            return false;
        }
        boost::python::extract<bool> result(adapted_.attr(
            PY_MAJOR_VERSION >= 3 ? "__bool__" : "__nonzero__"));
        return result.check() && boolean_type(result);*/

        return boolean_type(adapted_);
    }

 // void input (istream_type& in)        { boost::python::operator >>(in, adapted_); }
 // void output(ostream_type& out) const { boost::python::operator <<(out, adapted_); }
    void output(ostream_type& out) const {
        // boost::python::extract<string_type> const s(adapted_.attr("__str__"));
        boost::python::extract<string_type> const s((boost::python::str(adapted_)));

        if (!s.check()) {
            throw_exception(bad_method("str"));
        }
        out << string_type(s);
    }

    iterator end()   { return iterator(); }
    iterator begin() { return iterator(stl_iterator(adapted_)); }

    const_iterator end()   const { return const_iterator(); }
    const_iterator begin() const { return const_iterator(stl_iterator(adapted_)); }
};

}} // namespace ajg::synthesis

#endif // AJG_SYNTHESIS_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED
