//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_CHAR_TEMPLATE_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_CHAR_TEMPLATE_HPP_INCLUDED

#include <utility>
#include <cstring>

#ifndef AJG_SYNTH_NO_WCHAR_T
#    include <cwchar>
#endif

#include <ajg/synth/templates/base_template.hpp>

namespace ajg {
namespace synth {

template <class Engine>
struct char_template : base_template<Engine, typename Engine::traits_type::char_type const*> {
  public:

    typedef char_template                                                       template_type;
    typedef Engine                                                              engine_type;
    typedef typename engine_type::traits_type                                   traits_type;

    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;

    typedef std::pair<char_type const*, size_type>                              buffer_type;

  public:

    char_template(buffer_type const& buffer) : buffer_(buffer) {
        this->reset(this->buffer_.first, this->buffer_.first + this->buffer_.second);
    }

    char_template(char_type const* data, size_type const size) : buffer_(data, size) {
        this->reset(data, data + size);
    }

    char_template(char_type const* data) : buffer_(data, get_size(data)) {
        this->reset(data, data + this->buffer_.second);
    }

    // TODO: char_type[N] constructors that don't need to use get_size.

  public:

    buffer_type const& buffer() const { return this->buffer_; }

  private:

    buffer_type const buffer_;

    inline static size_type get_size(char const *const data) { return (std::strlen)(data); }

#ifndef AJG_SYNTH_NO_WCHAR_T
    inline static size_type get_size(wchar_t const *const data) { return (std::wcslen)(data); }
#endif

    template <class C>
    inline static size_type get_size(C const *const data) {
        size_type size = 0;
        while (*data++) size++;
        return size;
    }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_TEMPLATES_CHAR_TEMPLATE_HPP_INCLUDED
