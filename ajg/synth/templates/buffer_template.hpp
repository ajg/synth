//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_CHAR_TEMPLATE_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_CHAR_TEMPLATE_HPP_INCLUDED

#include <utility>

#ifndef AJG_SYNTH_CONFIG_NO_WCHAR_T
#    include <cwchar>
#endif

#include <ajg/synth/templates/base_template.hpp>

namespace ajg {
namespace synth {
namespace templates {

///
/// buffer_template:
///     A sibling of string_template that doesn't keep a local copy of the source, only a pointer
///     to it. This means the source MUST outlive the template or Bad Things will happen.
///
///     In general, prefer string_template over buffer_template unless you know what you're doing and
///     you've determined the latter provides a tangible net performance advantage over the former.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Engine>
struct buffer_template : base_template<Engine, typename Engine::traits_type::char_type const*> {
  public:

    typedef buffer_template                                                     template_type;
    typedef Engine                                                              engine_type;
    typedef typename engine_type::options_type                                  options_type;
    typedef typename options_type::traits_type                                  traits_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::buffer_type                                   buffer_type;

    typedef buffer_type                                                         source_type;
    typedef size_type                                                           key_type;

  public:

    inline buffer_template(source_type const& source, options_type const& options = options_type()) : source_(source) {
        this->reset(this->source_.first, this->source_.first + this->source_.second, options);
    }

    inline buffer_template(char_type const* data, size_type const size, options_type const& options = options_type()) : source_(data, size) {
        this->reset(data, data + size, options);
    }

    inline buffer_template(char_type const* data, options_type const& options = options_type()) : source_(data, infer_size(data)) {
        this->reset(data, data + this->source_.second, options);
    }

    template <size_type N>
    inline buffer_template(char_type const (&data)[N], options_type const& options = options_type()): source_(data, N) {
        this->reset(data, data + N, options);
    }

  public:

    inline buffer_type const& source() const { return this->source_; }

    inline static key_type const key(buffer_type const& source) { return source.second; }

    boolean_type const compatible(buffer_type const& source, options_type const& options) const {
        return this->source_ == source;
    }

    boolean_type const stale(buffer_type const& buffer, options_type const& options) const {
        AJG_SYNTH_ASSERT(this->compatible(buffer, options));
        return false;
    }

  private:

    inline static size_type infer_size(char const *const data) { return (std::strlen)(data); }

#ifndef AJG_SYNTH_CONFIG_NO_WCHAR_T
    inline static size_type infer_size(wchar_t const *const data) { return (std::wcslen)(data); }
#endif

    template <class C>
    inline static size_type infer_size(C const *const data) {
        size_type size = 0;
        while (*data++) size++;
        return size;
    }

  private:

    source_type const source_;
};

}}} // namespace ajg::synth::templates

#endif // AJG_SYNTH_TEMPLATES_CHAR_TEMPLATE_HPP_INCLUDED
