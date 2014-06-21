//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_PAIR_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_PAIR_HPP_INCLUDED

#include <utility>

#include <boost/variant.hpp>

#include <boost/iterator/iterator_facade.hpp>
#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for std::pair
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value, class First, class Second>
struct adapter<Value, std::pair<First, Second> >     : concrete_adapter<Value, std::pair<First, Second>, type_flags(container | sequential)> {
    adapter(std::pair<First, Second> const& adapted) : concrete_adapter<Value, std::pair<First, Second>, type_flags(container | sequential)>(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Value);

    virtual optional<boolean_type> get_boolean() const { return boolean_type(true); }
    virtual optional<range_type>   get_range() const {
        return range_type( const_pair_iterator(this->adapted(), first)
                         , const_pair_iterator(this->adapted(), past)
                         );
    }

    /* TODO:
    virtual boolean_type input(istream_type& istream) const {
        char_type a = 0, b = 0;
        value_type f(this->adapted().first);
        value_type s(this->adapted().second);
        return (istream >> f >> a >> b >> s) && (a == char_type(':') && b == char_type(' '));
    }
    */

    virtual boolean_type output(ostream_type& ostream) const {
        value_type const f(this->adapted().first);
        value_type const s(this->adapted().second);
        return ostream << f << ": " << s;
    }

  private:

    typedef std::pair<First, Second>        pair_type;
    typedef boost::variant<First, Second>   variant_type;

    enum position { first, second, past };

    template <class V>
    struct pair_iterator : boost::iterator_facade< pair_iterator<V>
                                                 , /* XXX: V */variant_type
                                                 , boost::forward_traversal_tag
                                                 , /* XXX: V */variant_type
                                                 > {
     public:
        pair_iterator( pair_type const& pair
                     , position  const  position
                     )
            : pair_(pair), position_(position) {}

        template <class T>
        pair_iterator(pair_iterator<T> const& other)
            : pair_(other.pair_), position_(other.position_) {}

        template <class T>
        bool equal(pair_iterator<T> const& that) const {
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

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_PAIR_HPP_INCLUDED
