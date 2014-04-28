//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_CLONE_PTR_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_CLONE_PTR_HPP_INCLUDED

#include <boost/smart_ptr/scoped_ptr.hpp>

namespace ajg {
namespace synth {
namespace detail {

template <class T>
struct clone_ptr : boost::scoped_ptr<T> {
  public:

    clone_ptr(T *const t = 0) : boost::scoped_ptr<T>(t) {}
    clone_ptr(clone_ptr const& c) : boost::scoped_ptr<T>(c ? &c->clone() : 0) {}

    inline clone_ptr& operator =(clone_ptr const& c) {
        this->reset(c ? &c->clone() : 0);
        return *this;
    }

    inline operator bool() const {
        return boost::scoped_ptr<T>::get() != 0;
    }
};

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_CLONE_PTR_HPP_INCLUDED
