//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_PAIR_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_PAIR_HPP_INCLUDED

#include <utility>

#include <boost/variant.hpp>

#include <boost/iterator/iterator_facade.hpp>
#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for std::pair
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class First, class Second>
struct adapter<Behavior, std::pair<First, Second> >
    : public base_adapter<Behavior> {

    typedef std::pair<First, Second> pair_type;
    typedef boost::variant<First, Second> variant_type;
    AJG_SYNTH_ADAPTER(pair_type)

  public:

    boolean_type to_boolean() const { return true; }
    void output(ostream_type& out) const { out << this->adapted_.first << ": " << this->adapted_.second; }
    boolean_type equal(base_type const& that) const { return this->equal_sequence(that); }

    const_iterator begin() const { return const_pair_iterator(adapted_, first); }
    const_iterator end()   const { return const_pair_iterator(adapted_, past); }

    adapted_type adapted_;

  private:

    enum position { first, second, past };

    template <class Value>
    struct pair_iterator : boost::iterator_facade< pair_iterator<Value>
                                                 , /* XXX: Value */variant_type
                                                 , boost::forward_traversal_tag
                                                 , /* XXX: Value */variant_type
                                                 > {
     public:
        pair_iterator( pair_type const& pair
                     , position  const  position
                     )
            : pair_(pair), position_(position) {}

        template <class Value_>
        pair_iterator(pair_iterator<Value_> const& other)
            : pair_(other.pair_), position_(other.position_) {}

        template <class Value_>
        bool equal(pair_iterator<Value_> const& that) const {
            return this->pair_     == that.pair_
                && this->position_ == that.position_;
        }

        void increment() {
                 if (position_ == first)  position_ = second;
            else if (position_ == second) position_ = past;
            else AJG_SYNTH_THROW(std::logic_error("invalid operation"));
        }

        variant_type dereference() const {
                 if (position_ == first)  return pair_.first;
            else if (position_ == second) return pair_.second;
            else AJG_SYNTH_THROW(std::logic_error("invalid iterator"));
        }

     private:
        friend class iterator_core_access;
        template <class> friend struct pair_iterator;

        pair_type const& pair_;
        position position_;
    };

    typedef pair_iterator<value_type const> const_pair_iterator;
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_PAIR_HPP_INCLUDED
