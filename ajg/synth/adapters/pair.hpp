//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

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

template <class Traits, class First, class Second>
struct adapter<Traits, std::pair<First, Second> >
    : public abstract_adapter<Traits> {

    typedef std::pair<First, Second> pair_type;
    typedef variant<First, Second> variant_type;
    AJG_SYNTH_ADAPTER(pair_type)

  public:

    // A pair can never be 'false', since its
    // 'length' is always non-zero (i.e. two.)
    boolean_type test() const { return true; }
    void output(ostream_type& out) const { this->list(out, traits_type::literal(": ")); }
    boolean_type equal(abstract_type const& that) const {
        return this->equal_sequence(that);
    }

    const_iterator begin() const { return const_pair_iterator(adapted_, first); }
    const_iterator end()   const { return const_pair_iterator(adapted_, past); }

    adapted_type adapted_;

  private:

    enum position { first, second, past };

    template <class Value>
    struct pair_iterator : iterator_facade
        <pair_iterator<Value>, /*Value*/variant_type, forward_traversal_tag, /*Value*/variant_type> {

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
            else throw_exception(std::logic_error("invalid operation"));
        }

        variant_type dereference() const {
                 if (position_ == first)  return pair_.first;
            else if (position_ == second) return pair_.second;
            else throw_exception(std::logic_error("invalid iterator"));
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
