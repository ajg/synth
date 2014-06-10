//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_BASE_VALUE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_BASE_VALUE_HPP_INCLUDED

#include <map>
#include <set>
#include <string>
#include <vector>
#include <iomanip>
#include <istream>
#include <ostream>
#include <sstream>
#include <utility>
#include <cstddef>
#include <iterator>
#include <algorithm>

#include <boost/bind.hpp>
#include <boost/config.hpp>
#include <boost/assert.hpp>
#include <boost/none_t.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_integral.hpp>

#include <ajg/synth/exceptions.hpp>
#include <ajg/synth/value_traits.hpp>
#include <ajg/synth/value_iterator.hpp>
#include <ajg/synth/adapters/numeric.hpp>
#include <ajg/synth/adapters/base_adapter.hpp>
#include <ajg/synth/detail/text.hpp>
#include <ajg/synth/detail/unmangle.hpp>
#include <ajg/synth/detail/container.hpp>
#include <ajg/synth/detail/has_fraction.hpp>

namespace ajg {
namespace synth {
namespace engines {

// TODO: Factor out the safe/token metadata into a separate (possibly sub-) class.
template <class Traits>
struct base_value {
  public:

    typedef base_value                                                          value_type;
    typedef Traits                                                              traits_type;

    typedef typename traits_type::none_type                                     none_type;
    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::integer_type                                  integer_type;
    typedef typename traits_type::floating_type                                 floating_type;
    typedef typename traits_type::number_type                                   number_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::symbols_type                                  symbols_type;
    typedef typename traits_type::date_type                                     date_type;
    typedef typename traits_type::time_type                                     time_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::duration_type                                 duration_type;
    typedef typename traits_type::timezone_type                                 timezone_type;
    typedef typename traits_type::istream_type                                  istream_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

    typedef value_iterator<value_type>                                          iterator;
    typedef value_iterator<value_type const>                                    const_iterator;
    typedef std::pair<const_iterator, const_iterator>                           range_type;

    typedef std::vector<value_type>                                             sequence_type;
    typedef std::map<string_type, value_type>                                   association_type;
    typedef std::pair<sequence_type, association_type>                          arguments_type;

    typedef boost::optional<integer_type>                                       index_type;

    typedef boost::optional<value_type>                                         attribute_type;
    typedef std::set<value_type>                                                attributes_type;

    typedef boost::shared_ptr<adapters::base_adapter<value_type> const>         adapter_type;

  private:

    typedef detail::text<string_type>                                           text;

  public:

    // An uninitialized value; in general to be avoided except where there's no better solution.
    base_value() : safe_(false), adapter_() {}

    template <class T>
    base_value(T const& t, typename boost::disable_if<boost::is_same<T, value_type> >::type* = 0)
        : safe_(false), adapter_(new adapters::adapter<value_type, T>(t)) {}

    template <class T, class U>
    base_value(T const& t, U const& u, typename boost::disable_if<boost::is_same<T, value_type> >::type* = 0)
        : safe_(false), adapter_(new adapters::adapter<value_type, T>(t, u)) {}

    template <class T, class U, class V>
    base_value(T const& t, U const& u, V const& v, typename boost::disable_if<boost::is_same<T, value_type> >::type* = 0)
        : safe_(false), adapter_(new adapters::adapter<value_type, T>(t, u, v)) {}

  public:

    inline boolean_type initialized()  const { return boolean_type(adapter_); }
    inline void         uninitialize()       { adapter_.reset(); }

  public: // TODO: Should only be visible to {default_}value_traits.

    inline adapters::type_flags  flags()     const { return this->adapter()->flags(); }
    inline std::type_info const& type()      const { return this->adapter()->type(); }
    inline string_type    const& type_name() const { return text::widen(detail::unmangle(this->type().name())); }

    // NOTE:
    //     is<T>: whether exactly T (modulo const/volatile)
    //     as<T>: cast to exactly T& (modulo const/volatile)
    //     to<T>: convert to T (i.e. a copy)

    template <class T> inline T const& as() const { BOOST_ASSERT(this->template is<T>()); T* t = this->adapter()->template get<T>(); BOOST_ASSERT(t); return *t; }
    template <class T> inline T&       as()       { BOOST_ASSERT(this->template is<T>()); T* t = this->adapter()->template get<T>(); BOOST_ASSERT(t); return *t; }

    template <class T> inline boolean_type is() const { return this->type() == typeid(T); }

    inline boolean_type is_unit()        const { return this->flags() & adapters::unit; }
    inline boolean_type is_boolean()     const { return this->flags() & adapters::boolean; }
    inline boolean_type is_character()   const { return this->flags() & adapters::character; }
    inline boolean_type is_textual()     const { return this->flags() & adapters::textual; }
    inline boolean_type is_floating()    const { return this->flags() & adapters::floating; }
    inline boolean_type is_integral()    const { return this->flags() & adapters::integral; }
    inline boolean_type is_numeric()     const { return this->flags() & adapters::numeric; }
    inline boolean_type is_chronologic() const { return this->flags() & adapters::chronologic; }
    inline boolean_type is_sequential()  const { return this->flags() & adapters::sequential; }
    inline boolean_type is_associative() const { return this->flags() & adapters::associative; }
    inline boolean_type is_container()   const { return this->flags() & adapters::container; }
    inline boolean_type is_iterable()    const { // TODO: Defer to adapter?
        return this->is_container() || this->is_sequential() || this->is_associative();
    }

    template <class T>
    inline T to(typename boost::enable_if<boost::is_same<T, value_type> >::type* = 0) const {
        return *this;
    }

    template <class T>
    inline T to(typename boost::disable_if<boost::is_same<T, value_type> >::type* = 0) const {
        // Exact match:
        if (this->template is<T>()) {
            return this->template as<T>();
        }

        T result;
        this->template into<T>(result);
        return result;
    }

    template <class T>
    inline void into(T& result) const {
        // Extremely crude implementation for the general case:
        /*
        try {
            return boost::lexical_cast<T, From>(from);
        }
        catch (boost::bad_lexical_cast const&) {
            AJG_SYNTH_THROW(conversion_error(typeid(From), typeid(To)));
        }
        */

        std::basic_stringstream<char_type> stream;
        if (this->adapter()->output(stream)) {
            BOOST_ASSERT(stream);
            if (stream >> result) {
                return;
            }

        }

        AJG_SYNTH_THROW(conversion_error(this->type(), typeid(T)));
    }

    inline boolean_type to_boolean() const {
        if (this->template is<boolean_type>()) {
            return this->template as<boolean_type>();
        }
        else if (this->is_unit()) {
            return false;
        }
        else if (boost::optional<boolean_type> const b = this->adapter()->get_boolean()) {
            return *b;
        }
        else if (boost::optional<number_type> const n = this->adapter()->get_number()) {
            return *n;
        }
        else if (boost::optional<range_type> const r = this->adapter()->get_range()) {
            return r->first != r->second;
        }
        return this->template to<boolean_type>();
    }

    inline size_type to_size() const {
        if (this->template is<size_type>()) {
            return this->template as<size_type>();
        }
        number_type const n = this->to_number();
        if (n <= 0) return 0;
        return static_cast<size_type>(n);
    }

    inline integer_type to_integer() const {
        if (this->template is<integer_type>()) {
            return this->template as<integer_type>();
        }
        return static_cast<integer_type>(this->to_number());
    }

    inline floating_type to_floating() const {
        if (this->template is<floating_type>()) {
            return this->template as<floating_type>();
        }
        return static_cast<floating_type>(this->to_number());
    }

    inline number_type to_number() const {
        if (this->template is<number_type>()) {
            return this->template as<number_type>();
        }
        else if (this->is_unit()) {
            return number_type(0);
        }
        else if (boost::optional<number_type> const n = this->adapter()->get_number()) {
            return *n;
        }
        else if (this->is_boolean()) {
            boolean_type const b = this->adapter()->get_boolean().get_value_or(boolean_type(false));
            return b ? number_type(1) : number_type(0); // TODO: Move this to traits.
        }
        return this->template to<number_type>();
    }

    inline datetime_type to_datetime() const {
        /* TODO: Once there's an adapter for datetime_type.
        if (this->template is<datetime_type>()) {
            return this->template as<datetime_type>();
        }
        else*/ if (boost::optional<datetime_type> const dt = this->adapter()->get_datetime()) {
            return *dt;
        }
        else if (this->is_unit()) {
            return traits_type::empty_datetime();
        }
        datetime_type dt = traits_type::empty_datetime();
        this->template into<datetime_type>(dt);
        return dt;
    }

    inline string_type to_string() const {
        if (this->template is<string_type>()) {
            return this->template as<string_type>();
        }
        else if (boost::optional<string_type> const s = this->adapter()->get_string()) {
            return *s;
        }
        std::basic_ostringstream<char_type> ss;
        operator<<(ss, *this);
        BOOST_ASSERT(ss);
        return ss.str();

        /*
        if (this->adapter()->output(ss)) {
            return ss.str();
        }

        AJG_SYNTH_THROW(invalid_method("to_string"));
        */
    }

    inline range_type to_range() const {
        if (this->template is<range_type>()) {
            return this->template as<range_type>();
        }
        else if (boost::optional<range_type> const r = this->adapter()->get_range()) {
            return *r;
        }
        return range_type(); // return this->template to<range_type>();
    }

    inline boolean_type contains(value_type const& that) const {
        return !this->find(that).equal(this->end()); // TODO: Defer to adapter first.
    }

    inline boolean_type equal(value_type const& that) const {
        if (this->type() == that.type())                          return this->adapter()->equal_to(that); // Exact.
        else if (this->is_boolean()     && that.is_boolean())     return this->to_boolean()  == that.to_boolean();
        else if (this->is_numeric()     && that.is_numeric())     return this->to_number()   == that.to_number();
        else if (this->is_chronologic() && that.is_chronologic()) return this->to_datetime() == that.to_datetime();
        else if (this->is_textual()     && that.is_textual())     return this->to_string()   == that.to_string();
        // TODO: Sequences, mappings, etc.?
        else return false;
    }

    inline boolean_type less(value_type const& that) const {
        if (this->type() == that.type())                          return this->adapter()->less(that); // Exact.
        else if (this->is_boolean()     && that.is_boolean())     return this->to_boolean()  < that.to_boolean();
        else if (this->is_numeric()     && that.is_numeric())     return this->to_number()   < that.to_number();
        else if (this->is_chronologic() && that.is_chronologic()) return this->to_datetime() < that.to_datetime();
        else if (this->is_textual()     && that.is_textual())     return this->to_string()   < that.to_string();
        // TODO: Sequences, mappings, etc.?
        else return false;
    }

    inline static void delimited(ostream_type& ostream, range_type const& range) {
        size_type i = 0;
        BOOST_FOREACH(value_type const& value, range) {
            i++ ? (ostream << ", " << value) : (ostream << value);
        }
    }

    inline size_type empty()  const { return this->size() == 0; }
    inline size_type size()   const {
        // TODO: Defer to adapter first.
        range_type const r = this->to_range();
        return std::distance(r.first, r.second);
    }

    inline value_type front() const { return *this->begin(); }
    inline value_type back()  const { return *this->at(-1); }

    // TODO: Defer to adapter first.
    inline const_iterator at   (value_type const& index) const { return detail::at(*this, index.to_integer()); }
    inline const_iterator find (value_type const& value) const { return this->adapter()->find(value); }

    // TODO: Fold into `at`?
    inline attribute_type attribute(value_type const& key) const {
        return this->adapter()->attribute(key);
    }

    inline void attribute(value_type const& key, attribute_type const& attribute) const {
        this->adapter()->attribute(key, attribute);
    }

    inline attributes_type attributes() const {
        return this->adapter()->attributes();
    }

    inline const_iterator begin() const { return this->to_range().first; }
    inline const_iterator end()   const { return this->to_range().second; }

    inline iterator begin() { return const_cast<base_value const*>(this)->begin(); }
    inline iterator end()   { return const_cast<base_value const*>(this)->end(); }


    inline range_type slice( index_type const lower = index_type()
                           , index_type const upper = index_type()
                           ) const { // TODO: Defer to adapter first.
        return detail::slice(*this, lower, upper);
    }

    inline operator boolean_type()                          const { return this->to_boolean(); }
    inline boolean_type operator!()                         const { return !this->to_boolean(); }
    inline boolean_type operator==(value_type const& that)  const { return this->equal(that); }
    inline boolean_type operator!=(value_type const& that)  const { return !this->equal(that); }
    inline boolean_type operator< (value_type const& that)  const { return this->less(that); }
    inline boolean_type operator<=(value_type const& that)  const { return !that.less(*this); }
    inline boolean_type operator> (value_type const& that)  const { return that.less(*this); }
    inline boolean_type operator>=(value_type const& that)  const { return !this->less(*this); }
    inline value_type   operator[](value_type const& index) const { return *this->at(index); }

  protected:

    inline adapter_type const& adapter() const {
        if (!this->adapter_) {
            AJG_SYNTH_THROW(std::logic_error("uninitialized value"));
        }
        return this->adapter_;
    }

  private:

    template <class V> friend struct value_iterator;

    template <class V> friend
    typename boost::enable_if<boost::is_same<value_type, V>, ostream_type&>::type
    operator <<(ostream_type& ostream, V const& value) {
        // TODO: Move non-output behavior to traits.
        if (value.is_unit()) {
            return ostream << "None";
        }
        else if (value.is_boolean()) {
            boolean_type const b = value.adapter()->get_boolean().get_value_or(boolean_type(false));
            return ostream << (b ? "True" : "False");
        }
        else if (value.adapter()->output(ostream)) {
            return ostream;
        }
        else if (boost::optional<string_type> const s = value.adapter()->get_string()) {
            return ostream << *s;
        }
        // else if (value.is_iterable()) {
        else if (boost::optional<range_type> const r = value.adapter()->get_range()) {
            return delimited(ostream, *r), ostream;
        }
        AJG_SYNTH_THROW(invalid_method("operator<<"));
    }

    template <class V> friend
    typename boost::enable_if<boost::is_same<value_type, V>, istream_type&>::type
    operator >>(istream_type& istream, value_type& value) {
        if (value.adapter()->input(istream)) {
            return istream;
        }
        AJG_SYNTH_THROW(invalid_method("operator>>"));
    }

  public:

    inline boolean_type safe() const { return this->safe_; }

    inline value_type& mark_unsafe() { return (this->safe_ = false), *this; }
    inline value_type& mark_safe() { return (this->safe_ = true), *this; }

    inline value_type&        token(string_type const& token) { return (this->token_ = token), *this; }
    inline string_type const& token() const { return this->token_; }

    inline boolean_type is_literal() const { return !this->token_.empty(); }

    // NOTE: This method does not copy the actual held value (in the adapter) just the metadata.
    inline value_type metacopy() const { return *this; }

    value_type escape() const {
        // XXX: Should this method escape binary and control characters?
        return text::escape_entities(this->to_string());
    }

    value_type must_get_attribute(value_type const& attribute) const {
        if (attribute_type const& attr = this->attribute(attribute)) {
            return *attr;
        }
        else {
            AJG_SYNTH_THROW(missing_attribute(text::narrow(attribute.to_string())));
        }
    }

    value_type must_get_trail(sequence_type const& trail) const {
        value_type value = *this;

        BOOST_FOREACH(value_type const& attribute, trail) {
            value = value.must_get_attribute(attribute);
        }

        return value;
    }

    typedef std::pair<value_type, sequence_type> group_type;
    typedef std::vector<group_type>              groups_type;

    groups_type group_by(value_type const& attrs) const {
        groups_type groups;
        value_type current_key;
        size_type i = 0;

        BOOST_FOREACH(value_type const& value, *this) {
            value_type const& key = value.must_get_trail(make_trail(attrs));

            // New group (either it's the first one or it has a different key.)
            if (!i++ || current_key != key) {
                current_key = key;
                groups.push_back(group_type(key, sequence_type(1, value)));
            }
            // Add to the current group.
            else {
                groups.back().second.push_back(value);
            }
        }

        return groups;
    }

    sequence_type to_sequence() const {
        sequence_type sequence;

        sequence.resize(this->size());
        size_type i = 0;
        BOOST_FOREACH(value_type const& value, *this) {
            sequence[i++] = value;
        }

        return sequence;
    }

    // NOTE: Returns a copy for now.
    value_type reverse() const {
        // TODO: Avoid copying the sequence for values with adapters that natively support rbegin/rend.
        sequence_type result;

        result.resize(this->size());
        size_type i = this->size() - 1;
        BOOST_FOREACH(value_type const& value, *this) {
            result[i--] = value;
        }

        return result;
    }

    // NOTE: Returns a copy for now.
    value_type sort_by(value_type const& attrs, boolean_type const reverse) const {
        // TODO: Avoid copying the sequence for values with adapters that natively support rbegin/rend.
        sequence_type result, trail = make_trail(attrs);

        result.reserve(this->size());
        BOOST_FOREACH(value_type const& value, *this) {
            result.push_back(value);
        }

        reverse ?
            std::sort(result.rbegin(), result.rend(), boost::bind(deep_less, boost::ref(trail), _1, _2)) :
            std::sort(result.begin(),  result.end(),  boost::bind(deep_less, boost::ref(trail), _1, _2));
        return result;
    }

  private:

    static boolean_type deep_less( sequence_type const& trail
                                 , value_type           a
                                 , value_type           b
                                 ) {
        return a.must_get_trail(trail) < b.must_get_trail(trail);
    }

    static sequence_type make_trail(value_type const& value) {
        string_type const source    = value.to_string();
        string_type const delimiter = text::literal(".");
        std::vector<string_type> const& names = text::split(source, delimiter);

        sequence_type trail;
        BOOST_FOREACH(string_type const& name, names) {
            trail.push_back(value_type(name));
        }
        return trail;
    }

  private:

    boolean_type safe_;
    string_type  token_;
    adapter_type adapter_;
};

}}} // namespace ajg::synth::engines

#endif // AJG_SYNTH_ENGINES_BASE_VALUE_HPP_INCLUDED
