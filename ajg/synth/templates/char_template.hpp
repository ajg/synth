//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_CHAR_TEMPLATE_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_CHAR_TEMPLATE_HPP_INCLUDED

#include <utility>
#include <cstring>

#ifndef AJG_SYNTH_CONFIG_NO_WCHAR_T
#    include <cwchar>
#endif

#include <ajg/synth/templates/base_template.hpp>

namespace ajg {
namespace synth {
namespace templates {

///
/// char_template:
///     A sibling of string_template that doesn't keep a local copy of the source, only a pointer
///     to it. This means the source MUST outlive the template or Bad Things will happen.
///
///     In general, prefer string_template over char_template unless you know what you're doing and
///     you've determined the latter provides a tangible net performance advantage over the former.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Engine>
struct char_template : base_template<Engine, typename Engine::traits_type::char_type const*> {
  public:

    typedef char_template                                                       template_type;
    typedef Engine                                                              engine_type;
    typedef typename engine_type::options_type                                  options_type;
    typedef typename options_type::traits_type                                  traits_type;

    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;

    typedef std::pair<char_type const*, size_type>                              buffer_type;

  public:

    inline char_template(buffer_type const& buffer) : buffer_(buffer) {
        this->reset(this->buffer_.first, this->buffer_.first + this->buffer_.second);
    }

    inline char_template(char_type const* data, size_type const size) : buffer_(data, size) {
        this->reset(data, data + size);
    }

    inline char_template(char_type const* data) : buffer_(data, get_size(data)) {
        this->reset(data, data + this->buffer_.second);
    }

    template <size_type N>
    inline char_template(char_type const (&data)[N]): buffer_(data, N) {
        this->reset(data, data + N);
    }

    inline char_template(buffer_type const& buffer, options_type const& options) : buffer_(buffer) {
        this->reset(this->buffer_.first, this->buffer_.first + this->buffer_.second, options);
    }

    inline char_template(char_type const* data, size_type const size, options_type const& options) : buffer_(data, size) {
        this->reset(data, data + size, options);
    }

    inline char_template(char_type const* data, options_type const& options) : buffer_(data, get_size(data)) {
        this->reset(data, data + this->buffer_.second, options);
    }

    template <size_type N>
    inline char_template(char_type const (&data)[N], options_type const& options): buffer_(data, N) {
        this->reset(data, data + N, options);
    }

  public:

    buffer_type const& buffer() const { return this->buffer_; }

  private:

    inline static size_type get_size(char const *const data) { return (std::strlen)(data); }

#ifndef AJG_SYNTH_CONFIG_NO_WCHAR_T
    inline static size_type get_size(wchar_t const *const data) { return (std::wcslen)(data); }
#endif

    template <class C>
    inline static size_type get_size(C const *const data) {
        size_type size = 0;
        while (*data++) size++;
        return size;
    }

  private:

    buffer_type const buffer_;
};

}}} // namespace ajg::synth::templates

#endif // AJG_SYNTH_TEMPLATES_CHAR_TEMPLATE_HPP_INCLUDED
