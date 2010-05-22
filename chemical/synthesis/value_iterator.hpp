
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_VALUE_ITERATOR_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_VALUE_ITERATOR_HPP_INCLUDED

#include <boost/smart_ptr/scoped_ptr.hpp>
#include <boost/iterator/iterator_facade.hpp>

namespace chemical {
namespace synthesis {
namespace detail {

template <class T>
struct clone_ptr : scoped_ptr<T> {
  public:

    clone_ptr(T* const t = 0) : scoped_ptr<T>(t) {}
    clone_ptr(clone_ptr const& c) : scoped_ptr<T>(c ? &c->clone() : 0) {}

    clone_ptr& operator =(clone_ptr const& c) {
        reset(c ? &c->clone() : 0);
        return *this;
    }
};

} // detail

//
// value_iterator
////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct value_iterator
    : iterator_facade< value_iterator<Value>
                     , Value
                     , forward_traversal_tag
                     , Value // This forces the 'reference' type to be
                     > {     // a real Value, not an actual reference.
  public:

 // typedef std::size_t size_type;
 // typedef Value       value_type;

  public:

    value_iterator() : iterator_() {}

    // explicit value_iterator(Value* const value) : value_(value) {}

    template <class Iterator>
    value_iterator(Iterator const& iterator)
        : iterator_(new polymorphic_iterator<Iterator>(iterator)) {}

    /*template <class Value_>
    value_iterator(value_iterator<Value_> const& other) : iterator_(new ...) {}
*/



  // private:

    template <class Value_>
    bool equal(value_iterator<Value_> const& that) const {
        return (!this->iterator_ && !that.iterator_)
            || this->iterator_->equal(*that.iterator_);
    }

    void increment() { iterator_->increment(); }
    void decrement() { iterator_->decrement(); }
    Value dereference() const { return iterator_->dereference(); }
    // value_iterator advance(size_type const distance) const;

  private:

    struct virtual_iterator {
        virtual void increment() = 0;
        virtual void decrement() = 0;
        virtual Value dereference() const = 0;
        virtual virtual_iterator& clone() const = 0;
        virtual bool equal(virtual_iterator const& that) const = 0;
        virtual ~virtual_iterator() {}
    };

    template <class Iterator>
    struct polymorphic_iterator : virtual_iterator {
        typedef polymorphic_iterator this_type;
        polymorphic_iterator(Iterator const& iterator) : iterator_(iterator) {}

        virtual void increment() { iterator_++; }
        virtual void decrement() { iterator_--; }
        virtual Value dereference() const { return *iterator_; }
        virtual this_type& clone() const { return *new this_type(iterator_); }
        virtual bool equal( virtual_iterator const& that) const {
         // BOOST_ASSERT(typeid(this_type) == typeid(that));
            BOOST_ASSERT(dynamic_cast<this_type const*>(&that));
            return static_cast<this_type const&>(that).iterator_ == this->iterator_;
        }

      private:
        Iterator iterator_;
    };

  private:

    friend class iterator_core_access;
    template <class> friend struct value_iterator;
    detail::clone_ptr<virtual_iterator> iterator_;
};


/*

template <class Iterator>
adaptable_iterator make_adaptable_iterator(Iterator const& iterator) {
    return adaptable_iterator(iterator);
}

template <class ValueIterator, class Iterator>
ValueIterator adapt(Iterator const& iterator) {
    return ValueIterator(iterator);
}
*/

}} // namespace chemical::synthesis

#endif // CHEMICAL_SYNTHESIS_VALUE_ITERATOR_HPP_INCLUDED
