
//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ENGINES_DJANGO_VALUE_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ENGINES_DJANGO_VALUE_HPP_INCLUDED

#include <utility>
#include <algorithm>

#include <boost/config.hpp>
#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include <boost/algorithm/string/classification.hpp>

#include <chemical/synthesis/value_facade.hpp>
#include <chemical/synthesis/adapters/numeric.hpp>

namespace chemical {
namespace synthesis {
namespace django {

template <class Iterator>
struct value : value_facade<typename Iterator::value_type, value<Iterator> > {
  public:

    typedef value                              this_type;
    typedef Iterator                           iterator_type;
    typedef typename Iterator::value_type      char_type;
    typedef value_facade<char_type, this_type> base_type;

    typedef typename base_type::size_type      size_type;
    typedef typename base_type::traits_type    traits_type;
    typedef typename base_type::string_type    string_type;
    typedef typename base_type::boolean_type   boolean_type;
    typedef typename base_type::const_iterator const_iterator;

  public:

    struct token_type : std::pair<iterator_type, iterator_type> {
        void assign( iterator_type const& begin
                   , iterator_type const& end
                   ) {
            this->first = begin;
            this->second = end;
        }
    };

  public:

    CHEMICAL_SYNTHESIS_VALUE_CONSTRUCTORS(value, base_type,
        BOOST_PP_COMMA() safe_(false) {})

  public:

    inline void safe(boolean_type const safe) { safe_ = safe; }
    inline boolean_type safe() const { return safe_; }

    inline this_type& mark_unsafe() {
        return this->safe(false), *this;
    }

    inline this_type mark_unsafe() const {
        this_type copy = *this;
        return copy.mark_unsafe();
    }

    inline this_type& mark_safe() {
        return this->safe(true), *this;
    }

    inline this_type mark_safe() const {
        this_type copy = *this;
        return copy.mark_safe();
    }

    template <class Token>
    inline void token(Token const& token) {
        token_ = token_type();
        token_->first = token.first;
        token_->second = token.second;
    }

    inline token_type const& token() const {
        BOOST_ASSERT(token_);
        return *token_;
    }

    inline boolean_type is_literal() const {
        return token_;
    }

    inline boolean_type is_numeric() const {
        typedef abstract_numeric_adapter<traits_type> numeric_adapter;
        return dynamic_cast<numeric_adapter const*>(this->get()) != 0;
    }

    inline boolean_type is_string() const {
        return this->template is<string_type>();
    }

    inline string_type to_string() const {
        return lexical_cast<string_type>(*this);
    }

    this_type escape() const {
        // xxx: Should this method escape binary and control characters?
        return detail::escape_entities(this->to_string());
    }

    const_iterator find_attribute(this_type const& attribute) const {
        try {
            // First try to find the value itself.
            return this->find(attribute);
        }
        catch (bad_method const& method) {
            if (method.name == "find") {
                try {
                    // If that fails, try using the value as an index.
                    return this->at(attribute.count());
                }
                catch (std::exception const&) {
                    // Do nothing, and pass through to the `throw' below,
                    // so that we surface the original `find' failure.
                }
            }

            throw;
        }
    }

  private:

    boolean_type safe_;
    optional<token_type> token_;
};

}}} // namespace chemical::synthesis::django

namespace chemical {
namespace synthesis {

//
// specialization for above
////////////////////////////////////////////////////////////////////////////////

/*template <class Traits, class Iterator>
struct adapter<Traits, django::value<Iterator> > {
    adapter(django::value<Iterator> const& value);
};*/

    /*: public forwarding_adapter<Traits, django::value<Iterator>, django::value<Iterator> > {

    adapter(scoped_array<T> const& adapted, size_t const length)
        : adapted_(adapted), length_(length) {}
    scoped_array<T> const& adapted_;
    size_t const length_;

    template <class A> A forward() const { return A(cref(*reinterpret_cast<T(*)[]>(adapted_.get())), length_); }
    // T (&get() const)[] { return *reinterpret_cast<T(*)[]>(adapted_.get()); }
    bool valid() const { return adapted_; }
};*/

}} // namespace chemical::synthesis

#endif // CHEMICAL_SYNTHESIS_ENGINES_DJANGO_VALUE_HPP_INCLUDED
