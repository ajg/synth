//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_VALUE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_VALUE_HPP_INCLUDED

#include <vector>
#include <utility>
#include <algorithm>

#include <boost/bind.hpp>
#include <boost/config.hpp>
#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <ajg/synth/value_facade.hpp>
#include <ajg/synth/adapters/numeric.hpp>
#include <ajg/synth/engines/exceptions.hpp>

namespace ajg {
namespace synth {
namespace django {

template <class Traits>
struct value : value_facade<Traits, value> {
  public:

    typedef value                                                               value_type;
    typedef Traits                                                              traits_type;
    typedef typename value_type::facade_type                                    facade_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::number_type                                   number_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::string_type                                   string_type;

    typedef typename value_type::const_iterator                                 const_iterator;

    typedef std::vector<value_type>                                             sequence_type; // TODO: Derive from behavior_type.
    typedef std::map<string_type, value_type>                                   mapping_type; // TODO: Derive from behavior_type.

  public:

    value() : facade_type(), safe_(false), token_(boost::none) {}
    template <class T> value(T const& t) : facade_type(t), safe_(false), token_(boost::none) {}
    value(value const& that) : facade_type(that), safe_(that.safe_), token_(that.token_) {} // TODO[c++11] = default.

  public:

    inline value_type copy() const { return *this; }

    inline value_type&  safe(boolean_type const safe) { return (this->safe_ = safe), *this; }
    inline boolean_type safe() const { return this->safe_; }

    inline value_type& mark_unsafe() { return this->safe(false), *this; }
    inline value_type& mark_safe() { return this->safe(true), *this; }

    inline value_type&        token(string_type const& token) { return (this->token_ = token), *this; }
    inline string_type const& token() const { BOOST_ASSERT(this->token_); return *this->token_; }

    inline boolean_type is_literal() const { return boolean_type(this->token_); }

	value_type escape() const {
        // XXX: Should this method escape binary and control characters?
        return detail::escape_entities(this->to_string());
    }

    /*
    const_iterator find_attribute(value_type const& attribute) const {
        try {
            // First try to find the value itself.
            return this->find(attribute);
        }
        catch (bad_method const& method) {
            if (method.name == "find") {
                try {
                    // If that fails, try using the value as an index.
                    return this->at(attribute.to_number());
                }
                catch (std::exception const&) {
                    // Do nothing, and pass through to the `throw' below,
                    // so that we surface the original `find' failure.
                }
            }

            throw;
        }
    }
    */

    // TODO: Move this to traits.
    optional<value_type> get_attribute(value_type const& attribute) const {
        try {
            // First try to locate the value as a key.
            return this->index(attribute);
        }
        catch (bad_method const& method) {
            if (method.name == "index") {
                try {
                    // If that fails, try using the value as an index.
                    const_iterator const it = this->at(attribute.to_number());
                    return it == this->end() ? none : *it;
                }
                catch (std::exception const&) {
                    // Do nothing, and pass through to the `throw' below,
                    // so that we surface the original `index' failure.
                }
            }

            throw;
        }
    }

    value_type must_get_attribute(value_type const& attribute) const {
        if (optional<value_type> attr = this->get_attribute(attribute)) {
            return *attr;
        }
        else {
            throw_exception(missing_attribute(traits_type::narrow(attribute.to_string())));
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
        namespace algo = boost::algorithm;

        std::vector<string_type> names;
        string_type const source = value.to_string(), delimiter = traits_type::literal(".");
        algo::split(names, source, algo::is_any_of(delimiter));

        sequence_type trail;
        BOOST_FOREACH(string_type const& name, names) {
            trail.push_back(value_type(name));
        }
        return trail;
    }

  private:

    boolean_type          safe_;
    optional<string_type> token_;
};

}}} // namespace ajg::synth::django

#endif // AJG_SYNTH_ENGINES_DJANGO_VALUE_HPP_INCLUDED
