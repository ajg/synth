//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_BASE_CONTEXT_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_BASE_CONTEXT_HPP_INCLUDED

#include <map>
#include <utility>

#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>

#include <ajg/synth/exceptions.hpp>
#include <ajg/synth/detail/find.hpp>

namespace ajg {
namespace synth {
namespace engines {

//
// base_context
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct base_context /*: boost::noncopyable*/ {
  public:

    typedef Value                                                               key_type;
    typedef Value                                                               value_type;
    typedef base_context                                                        base_context_type;

    typedef typename value_type::range_type                                     range_type;
    typedef typename value_type::sequence_type                                  sequence_type;
    typedef typename value_type::association_type                               association_type;
    typedef typename value_type::attribute_type                                 attribute_type;
    typedef typename value_type::attributes_type                                attributes_type;
    typedef typename value_type::traits_type                                    traits_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::integer_type                                  integer_type;
    typedef typename traits_type::floating_type                                 floating_type;
    typedef typename traits_type::number_type                                   number_type;
    typedef typename traits_type::date_type                                     date_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::duration_type                                 duration_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::symbols_type                                  symbols_type;
    typedef typename traits_type::paths_type                                    paths_type;
    typedef typename traits_type::names_type                                    names_type;
    typedef typename traits_type::istream_type                                  istream_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

  public: // TODO: private:

    typedef size_type                                                           marker_type; // FIXME: pair<filename, size_type>
    typedef std::map<string_type, string_type>                                  blocks_type;
    typedef std::map<marker_type, size_type>                                    cycles_type;
    typedef std::map<marker_type, value_type>                                   changes_type;

  private:

    typedef detail::text<string_type>                                           text;

  public:

    inline base_context(value_type const& value) : value_(value), autoescape_(true), blocks_(0) {}

  public:

    inline void set(key_type const& key, value_type const& value) { this->value_.attribute(this->cased(key), value); }

    inline void unset(key_type const& key) { this->value_.attribute(this->cased(key), attribute_type()); }

    inline attribute_type get(key_type const& key) const { return this->value_.attribute(this->cased(key)); }

    inline boolean_type has(key_type const& key) const { return detail::contains(this->cased(key), this->keys()); }

    inline attributes_type keys() const { return this->value_.attributes(); }

  // protected:

    inline value_type const& value() const { return this->value_; }

  private:

    inline key_type cased(key_type const& original) const {
        if (this->case_sensitive) {
            return original;
        }
        string_type const lowercased = text::lower(original.to_string());

        BOOST_FOREACH(key_type const& key, this->keys()) {
            if (text::lower(key.to_string()) == lowercased) {
                return key;
            }
        }

        return original;
    }

  public: // TODO: private:

    inline boolean_type top_level() const {
        return this->blocks_ == 0;
    }

    inline boost::optional<string_type> get_block(string_type const& name) const {
        if (this->top_level()) {
            AJG_SYNTH_THROW(std::invalid_argument("not in a derived template"));
        }
        return detail::find(name, *this->blocks_);
    }

    inline string_type get_base_block() const {
        if (this->base_block_.empty()) {
            AJG_SYNTH_THROW(std::invalid_argument("not in a derived block"));
        }
        else if (boost::optional<string_type> const& block = this->get_block(this->base_block_)) {
            return *block;
        }
        else {
            AJG_SYNTH_THROW(std::logic_error("invalid block"));
        }
    }

  public:

    boolean_type  case_sensitive;

  public: // private:

    value_type    value_;
    boolean_type  autoescape_;
    blocks_type*  blocks_;
    cycles_type   cycles_;
    changes_type  changes_;
    string_type   base_block_;
};

template <class Context>
struct stage : boost::noncopyable {
  public:

    typedef Context                                                             context_type;

    typedef typename context_type::key_type                                     key_type;
    typedef typename context_type::value_type                                   value_type;

    typedef typename value_type::attribute_type                                 attribute_type;
    typedef typename value_type::attributes_type                                attributes_type;
    typedef typename value_type::traits_type                                    traits_type;

    typedef typename traits_type::boolean_type                                  boolean_type;


  private:

    typedef std::map<key_type, attribute_type>                                  previous_type;

  public:

    stage(context_type& context) : context_(context) {}
    stage(context_type& context, key_type const& key, value_type const& value) : context_(context) { this->set(key, value); }
    stage(context_type& context, boolean_type const empty) : context_(context) { if (empty) this->clear(); }

    ~stage() {
        BOOST_FOREACH(typename previous_type::value_type const& kv, this->previous_) {
            if (kv.second) {
                this->context_.set(kv.first, *kv.second);
            }
            else {
                this->context_.unset(kv.first);
            }
        }
    }

  public:

    inline void clear() {
        BOOST_FOREACH(key_type const& key, context_.keys()) {
            this->unset(key);
        }
    }

    inline void unset(key_type const& key) {
        if (!detail::contains(key, this->previous_)) {
            this->previous_[key] = this->context_.get(key);
        }
        this->context_.unset(key);
    }

    inline void set(key_type const& key, value_type const& value) {
        if (!detail::contains(key, this->previous_)) {
            this->previous_[key] = this->context_.get(key);
        }
        this->context_.set(key, value);
    }

  private:

    context_type& context_;
    previous_type previous_;
};

}}} // namespace ajg::synth::engines

#endif // AJG_SYNTH_ENGINES_BASE_CONTEXT_HPP_INCLUDED

