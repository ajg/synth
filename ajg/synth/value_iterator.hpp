//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_VALUE_ITERATOR_HPP_INCLUDED
#define AJG_SYNTH_VALUE_ITERATOR_HPP_INCLUDED

#include <boost/iterator/iterator_facade.hpp>

#include <ajg/synth/detail/clone_ptr.hpp>

namespace ajg {
namespace synth {

//
// value_iterator
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct value_iterator
    : boost::iterator_facade< value_iterator<Value>
                            , Value
                            , boost::forward_traversal_tag
                            , Value // This forces the 'reference' type to be
                            > {     // a real Value, not an actual reference.
  public:

 // typedef std::size_t size_type;
 // typedef Value       value_type;

  public:

    value_iterator() : iterator_() {}

    // explicit value_iterator(Value *const value) : value_(value) {}

    template <class ForwardIterator>
    value_iterator(ForwardIterator const& iterator)
        : iterator_(new polymorphic_iterator<ForwardIterator>(iterator)) {}

    /*template <class V>
    value_iterator(value_iterator<V> const& other) : iterator_(new ...) {}
*/



  // private:

    template <class V>
    bool equal(value_iterator<V> const& that) const {
        bool const a = this->iterator_, b = that.iterator_;
        return (!a && !b) || (a && b && this->iterator_->equal(*that.iterator_));
    }

    void increment() { AJG_SYNTH_ASSERT(iterator_); iterator_->increment(); }
    Value dereference() const { AJG_SYNTH_ASSERT(iterator_); return iterator_->dereference(); }
    // value_iterator advance(size_type const distance) const;

  private:

    struct virtual_iterator {
        virtual void increment() = 0;
        virtual Value dereference() const = 0;
        virtual virtual_iterator& clone() const = 0;
        virtual bool equal(virtual_iterator const& that) const = 0;
        virtual ~virtual_iterator() {}
    };

    template <class ForwardIterator>
    struct polymorphic_iterator : virtual_iterator {
        polymorphic_iterator(ForwardIterator const& iterator) : iterator_(iterator) {}

        virtual void increment() { iterator_++; }
        virtual Value dereference() const { return *iterator_; }
        virtual polymorphic_iterator& clone() const { return *new polymorphic_iterator(iterator_); }
        virtual bool equal(virtual_iterator const& that) const {
         // AJG_SYNTH_ASSERT(typeid(polymorphic_iterator) == typeid(that));
            AJG_SYNTH_ASSERT(dynamic_cast<polymorphic_iterator const*>(&that));
            return static_cast<polymorphic_iterator const&>(that).iterator_ == this->iterator_;
        }

      private:
        ForwardIterator iterator_;
    };

  private:

    friend class boost::iterator_core_access;
    template <class> friend struct value_iterator;
    detail::clone_ptr<virtual_iterator> iterator_;
};


/*

template <class ForwardIterator>
adaptable_iterator make_adaptable_iterator(ForwardIterator const& iterator) {
    return adaptable_iterator(iterator);
}

template <class ValueIterator, class ForwardIterator>
ValueIterator adapt(ForwardIterator const& iterator) {
    return ValueIterator(iterator);
}
*/

}} // namespace ajg::synth

#endif // AJG_SYNTH_VALUE_ITERATOR_HPP_INCLUDED
