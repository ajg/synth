//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_MUTABLE_ATOMIC_SINGLETON_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_MUTABLE_ATOMIC_SINGLETON_HPP_INCLUDED

#include <boost/atomic.hpp>
#include <boost/optional.hpp>

namespace ajg {
namespace synth {
namespace detail {

// Note: In theory header-safe due to extern inline [per 7.1.2/4].
//
// extern inline T getset(boost::optional<T> const v = boost::none) {
//     static T value = 0;
//     return v ? (value = *v) : value;
// }

// Note: In theory header-safe and atomic.
//
// extern inline T getset(boost::optional<T> const v = boost::none) {
//     static boost::atomic<T> value = 0;
//     return v ? value.store(*v, MemoryOrder), *v : value.load(MemoryOrder);
// }

///
/// mutable_atomic_singleton
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, boost::memory_order MemoryOrder = boost::memory_order_seq_cst>
struct mutable_atomic_singleton {
  public:

    inline static void set(T const& t) { getset(t); }
    inline static T get() { return getset(boost::none);  }

  private:

    /* XXX: extern */ inline static T getset(boost::optional<T> const& v) {
        static boost::atomic<T> value;
        return v ? value.store(*v, MemoryOrder), *v : value.load(MemoryOrder);
    }
};

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_MUTABLE_ATOMIC_SINGLETON_HPP_INCLUDED
