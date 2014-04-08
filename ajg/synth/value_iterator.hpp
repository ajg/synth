//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_VALUE_ITERATOR_HPP_INCLUDED
#define AJG_SYNTH_VALUE_ITERATOR_HPP_INCLUDED

#include <boost/smart_ptr/scoped_ptr.hpp>
#include <boost/iterator/iterator_facade.hpp>

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

} // detail

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

    /*template <class Value_>
    value_iterator(value_iterator<Value_> const& other) : iterator_(new ...) {}
*/



  // private:

    template <class Value_>
    bool equal(value_iterator<Value_> const& that) const {
        bool const a = this->iterator_, b = that.iterator_;
        return (!a && !b) || (a && b && this->iterator_->equal(*that.iterator_));
    }

    void increment() { BOOST_ASSERT(iterator_); iterator_->increment(); }
    Value dereference() const { BOOST_ASSERT(iterator_); return iterator_->dereference(); }
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
        typedef polymorphic_iterator this_type;
        polymorphic_iterator(ForwardIterator const& iterator)
            : iterator_(iterator) {}

        virtual void increment() { iterator_++; }
        virtual Value dereference() const { return *iterator_; }
        virtual this_type& clone() const { return *new this_type(iterator_); }
        virtual bool equal(virtual_iterator const& that) const {
         // BOOST_ASSERT(typeid(this_type) == typeid(that));
            BOOST_ASSERT(dynamic_cast<this_type const*>(&that));
            return static_cast<this_type const&>(that).iterator_ == this->iterator_;
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
