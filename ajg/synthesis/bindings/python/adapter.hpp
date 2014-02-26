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

    typedef typename boost::python::stl_input_iterator<object_type> stl_iterator;

  public:

    inline static string_type as_string(boost::python::object const& obj) {
        boost::python::extract<string_type> const s((boost::python::str(obj)));
        if (!s.check()) {
            throw_exception(bad_method("str"));
        }
        return string_type(s);
    }

    boolean_type test() const { return boolean_type(adapted_); }

 // void input (istream_type& in)        { in >> adapted_; }
 // void output(ostream_type& out) const { out << adapted_; }
    void output(ostream_type& out) const { out << as_string(adapted_); }

    iterator begin() { return iterator(stl_iterator(adapted_)); }
    iterator end()   { return iterator(stl_iterator()); }

    const_iterator begin() const { return const_iterator(stl_iterator(adapted_)); }
    const_iterator end()   const { return const_iterator(stl_iterator()); }

    optional<value_type> index(value_type const& key) const {
        string_type const k = key.to_string();
        AJG_DUMP(k);
        boost::python::str kk((k));

        AJG_DUMP(as_string(adapted_));

        // Per https://docs.djangoproject.com/en/dev/topics/templates/#variables
        // TODO: Move this to django::engine.
        // 1. Dictionary lookup
        /*boost::python::extract<boost::python::dict> const d((adapted_));
        AJG_DUMP(d.check());

        if (d.check()) {
            boost::python::dict const& dict((d));
            AJG_DUMP(dict.has_key(kk));
            if (dict.has_key(kk)) {
                return value_type(boost::python::object(dict[kk]));
            }
        }
        */
        PyObject* o = adapted_.ptr();
        AJG_DUMP(PyMapping_HasKeyString(o, const_cast<char*>(k.c_str())));

        if (PyMapping_HasKeyString(o, const_cast<char*>(k.c_str()))) {
            return value_type(boost::python::object(adapted_[kk]));
        }

        AJG_DUMP(PyObject_HasAttrString(o, k.c_str()));

        // 2. Attribute lookup
        // return value_type(boost::python::object(adapted_.attr(kk)));
        if (!PyObject_HasAttrString(o, k.c_str())) {
            return optional<value_type>(); // return none;
        }

        boost::python::object obj = adapted_.attr(kk);
        AJG_DUMP(as_string(obj));

        // 3. Method call
        if (PyCallable_Check(obj.ptr())) {
            obj = obj();
        }
        AJG_DUMP(as_string(obj));
        return value_type(obj);


        // 4. TODO: List-index lookup
    }

    #if 0
    const_iterator find(value_type const& value) const {
        // Per https://docs.djangoproject.com/en/dev/topics/templates/#variables
        // 1. Dictionary lookup
        boost::python::extract<boost::python::dict const&> d((adapted_));

        if (d.check()) {
          boost::python::dict const& dict((d));
          const_iterator it = dict.find(value), end(dict.end());
          if (it != end) {
              return const_iterator(it);
          }
        }

        /*const_iterator it = adapted.attr(value);

        // 2. Attribute lookup
        if (it != end) {
            return it;
        }*/

        // 3. TODO: Method call
        // 4. TODO: List-index lookup

        return const_iterator(end);
    }
    #endif
};

}} // namespace ajg::synthesis

#endif // AJG_SYNTHESIS_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED
