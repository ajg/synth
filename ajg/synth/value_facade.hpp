//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_VALUE_FACADE_HPP_INCLUDED
#define AJG_SYNTH_VALUE_FACADE_HPP_INCLUDED

#include <cstddef>
#include <utility>
#include <iterator>

#include <boost/none_t.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ajg/synth/value_traits.hpp>
#include <ajg/synth/adapters/numeric.hpp>
#include <ajg/synth/adapters/abstract.hpp>

// #include <boost/spirit/home/support/safe_bool.hpp>
#include <boost/spirit/home/classic/core/safe_bool.hpp>


namespace ajg {
namespace synth {
namespace detail {

// Accepts the indices for a half-open range [lower, upper)
// and returns said range as a pair of iterators; imitates
// python's sequence slicing, including negative indices,
// which are "rotated" into their positive counterparts.
// The indices are bounds-checked regardless of their sign.
template <class Containter>
inline std::pair< typename Containter::const_iterator
                , typename Containter::const_iterator
                >
slice( Containter const& container
     , optional<int> const lower = none
     , optional<int> const upper = none
     ) {
    typedef typename Containter::size_type size_type;
    size_type const size = container.size();
    int lower_ = lower.get_value_or(0);
    int upper_ = upper.get_value_or(size);

    // Adjust negative indices to the right position.
    if (lower_ < 0) lower_ = static_cast<int>(size) + lower_;
    if (upper_ < 0) upper_ = static_cast<int>(size) + upper_;

    // Check for indices that are out of range.
    if (lower_ < 0 || size_type(lower_) > size)
        throw_exception(std::out_of_range("lower index"));
    if (upper_ < 0 || size_type(upper_) > size)
        throw_exception(std::out_of_range("upper index"));
    if (lower_ > upper_)
        throw_exception(std::logic_error("reversed indices"));

    // Move to the right places.
    typename Containter::const_iterator first = container.begin();
    typename Containter::const_iterator second = first;
    std::advance(first, lower_);
    std::advance(second, upper_);
    return std::make_pair(first, second);
}

} // detail


//
// forward declaration of adapter
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Char, class Adapted>
struct adapter;

//
// value_facade
////////////////////////////////////////////////////////////////////////////////////////////////////

template < class Char
         , class Value
         , class Traits = default_value_traits<Char, Value>
         >
struct value_facade : spirit::classic::safe_bool<value_facade<Char, Value> > {
  public:

    typedef value_facade             this_type;
    typedef Value                    value_type;
    typedef Traits                   traits_type;
    typedef abstract_adapter<Traits> abstract_type;

    typedef typename traits_type::size_type     size_type;
    typedef typename traits_type::char_type     char_type;
    typedef typename traits_type::range_type    range_type;
    typedef typename traits_type::string_type   string_type;
    typedef typename traits_type::number_type   number_type;
    typedef typename traits_type::datetime_type datetime_type;
    typedef typename traits_type::duration_type duration_type;
    typedef typename traits_type::boolean_type  boolean_type;
    typedef typename traits_type::istream_type  istream_type;
    typedef typename traits_type::ostream_type  ostream_type;

    typedef typename traits_type::iterator       iterator;
    typedef typename traits_type::const_iterator const_iterator;

  protected: // public:

    // An 'empty' value.
    value_facade() : adapter_() {}
    value_facade(none_t const&) : adapter_() {}

    template <class T>
    value_facade(T const& t,
    typename disable_if<boost::is_same<T, value_type> >::type* = 0)
        : adapter_(new adapter<Traits, T>(t)) {}

    template <class T, class U>
    value_facade(T const& t, U const& u,
    typename disable_if<boost::is_same<T, value_type> >::type* = 0)
        : adapter_(new adapter<Traits, T>(t, u)) {}

    template <class T, class U, class V>
    value_facade(T const& t, U const& u, V const& v,
    typename disable_if<boost::is_same<T, value_type> >::type* = 0)
        : adapter_(new adapter<Traits, T>(t, u, v)) {}

    /*template <class Adapter>
    inline static value_facade adapt(Adapter const& adapter) {
        BOOST_MPL_ASSERT(( is_base_of<abstract_type, Adapter> ));
        value_type value;
        value.adapter_.reset(new Adapter(adapter));
        return value;
    }*/

  public:

    inline boolean_type is_numeric() const {
        typedef abstract_numeric_adapter<traits_type> numeric_adapter;
        return dynamic_cast<numeric_adapter const*>(this->get()) != 0;
    }

    inline boolean_type is_string() const {
        return this->template is<string_type>();
    }

    inline string_type to_string() const {
        return get()->to_string();
    }

    inline void          clear()       { return adapter_.reset(); }
    inline number_type   count() const { return get()->count(); }
    inline datetime_type to_datetime() const { return get()->to_datetime(); }

    inline std::type_info const& type() const { return get()->type(); }

    inline boolean_type equal(value_type const& that) const {
        return this->get()->equal(*that.get());
    }

    // TODO: Make overridable by specializations (e.g. map to `in` in python.)
    inline boolean_type contains(value_type const& that) const {
        return !this->find(that).equal(this->end());
    }

    inline boolean_type empty() const { return !adapter_; }
    // TODO: Make overridable by specializations (e.g. map to __len__ in python.)
    inline size_type size() const { return std::distance(begin(), end()); }
    // TODO: Consider getting rid of length() and changing clients to size().
    inline size_type length() const { return this->size(); }

    // Even the non-const versions are immutable, and
    // are provided simply as a convenience interface.
    inline iterator begin() { return const_cast<this_type const*>(this)->begin(); }
    inline iterator end()   { return const_cast<this_type const*>(this)->end(); }

    inline const_iterator begin() const { return get()->begin(); }
    inline const_iterator end()   const {
        try {
            return get()->end();
        }
        catch (bad_method const& method) {
            return method.name == "end" ?
                const_iterator() : throw;
        }
    }

    inline range_type slice(
            optional<int> const lower = none,
            optional<int> const upper = none) const {
        return detail::slice(*this, lower, upper);
    }

    inline value_type front() const {
        return *this->begin();
    }

    inline value_type back() const {
        // return *--this->end();
        return *this->at(-1);
    }

    inline const_iterator at(/*value_type*/number_type index) const {
        // TODO: Once we have value_iterator::advance_to
        // consider using return begin() + index, to be O(1).

        // TODO: This behavior is specific to sequential containers
        //       (e.g. lists) not associative ones (like maps).
        if (index < 0) {
            index += this->length();
        }

        // For now, we must use this O(n) method:
        const_iterator it = begin();
        const_iterator const end = this->end();

        for (size_type i = 0; it != end; ++it, ++i) {
            if (/*value_type*/(i) == index) {
                return it;
            }
        }

        throw_exception(std::invalid_argument("index"));
    }

    inline const_iterator find(value_type const& value) const {
        return get()->find(value);
    }

    inline optional<value_type> index(value_type const& key) const {
        return get()->index(key);
    }

  public:

    inline boolean_type operator_bool() const { return !empty() && get()->test(); }
    inline boolean_type operator !()    const { return !operator_bool(); }

    inline boolean_type operator ==(value_type const& that) const {
        // Two empty values equal each other.
        return (!this->adapter_ && !that.adapter_)
            || ((this->adapter_ && that.adapter_)
              && this->adapter_->equal(*that.adapter_));
    }

    inline boolean_type operator !=(value_type const& that) const {
        return !this->operator ==(that);
    }

    inline boolean_type operator <(value_type const& that) const {
        if (!this->adapter_ && that.adapter_) {
            return true;
        }
        else {
            return this->adapter_ && that.adapter_ && this->adapter_->less(*that.adapter_);
        }
    }

    inline boolean_type operator <=(value_type const& that) const {
        return !this->operator >(that);
    }

    inline boolean_type operator >(value_type const& that) const {
        return that.operator <(*this);
    }

    inline boolean_type operator >=(value_type const& that) const {
        return that.operator <=(*this);
    }

    inline value_type const operator [](/*value_type*/number_type const& index) const {
        return *this->at(index);
    }

  protected:

    template <class T>
    inline boolean_type is() const {
        return get()->type() == typeid(T);
    }

    inline const abstract_type* get() const {
        if (!adapter_) {
            throw_exception(std::logic_error("empty value"));
        }
        return adapter_.get();
    }

  private:

    template <class V> friend struct value_iterator;
    template <class C, class V, class T> friend struct value_facade;

    friend ostream_type& operator <<(ostream_type& output, value_type const& value) {
        if (value.empty()) {
            return output << boost::lexical_cast<string_type>("<empty>");
        }
        else {
            return value.get()->output(output), output;
        }
    }

    friend istream_type& operator >>(istream_type& input, value_type& value) {
        return value.get()->input(input), input;
    }

  private:

    boost::shared_ptr<abstract_type const> adapter_;
};

#define AJG_SYNTH_VALUE_CONSTRUCTORS(name, base, rest) \
    name() \
        : base() rest \
    \
    name(none_t const&) \
        : base() rest \
    \
    template <class T> \
    name(T const& t) \
        : base(t) rest \
    \
    template <class T, class U> \
    name(T const& t, U const& u) \
        : base(t, u) rest \
    \
    template <class T, class U, class V> \
    name(T const& t, U const& u, V const& v) \
        : base(t, u, v) rest

}} // namespace ajg::synth

#endif // AJG_SYNTH_VALUE_FACADE_HPP_INCLUDED
