//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_DETAIL_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_DETAIL_HPP_INCLUDED

#include <ajg/synth/config.hpp>

#include <limits>
#include <vector>
#include <iterator>

namespace ajg {
namespace synth {
namespace detail {

template <class InputStream>
struct bidirectional_input_stream {
  public:

    typedef InputStream                             input_stream_type;
    typedef typename input_stream_type::char_type   char_type;
    typedef typename input_stream_type::off_type    position_type;

  public:

    struct iterator {

      public:
        typedef char_type                           value_type;
        typedef char_type const*                    pointer;
        typedef char_type const&                    reference;
        typedef position_type                       difference_type;
        typedef std::bidirectional_iterator_tag     iterator_category;

        iterator() : stream_(0), position_(0) {} // For Xpressive.
        iterator(bidirectional_input_stream* const stream, position_type const position)
            : stream_(stream), position_(position) {}

        bool      operator==(iterator const& other) const { return this->at(other.position_); }
        bool      operator< (iterator const& other) const { return this->position_ < other.position_; }
        bool      operator!=(iterator const& other) const { return !(*this == other); }
        iterator& operator++()      { ++this->position_; return *this; }
        iterator  operator++(int)   { iterator rc(*this); this->operator++(); return rc; }
        iterator  operator--(int)   { iterator rc(*this); this->operator--(); return rc; }
        iterator& operator--()      { this->maybe_read(); --this->position_; return *this; }
        char_type operator*() const { return this->stream_->get(this->position_); }

      private:

        inline bool at(position_type const position) const {
            return this->position_ == position || (this->position_ == this->stream_->current_size()
                && !this->stream_->expand() && position == (std::numeric_limits<position_type>::max)());
        }

        inline void maybe_read() {
            if (this->position_ == (std::numeric_limits<position_type>::max)()) {
                this->position_ = this->stream_->read_all();
            }
        }

        bidirectional_input_stream* stream_;
        position_type               position_;
    };

    typedef iterator                              const_iterator;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  public:

    bidirectional_input_stream(input_stream_type& stream): stream_(stream) {}

    iterator         begin()  { return iterator(this, 0); }
    iterator         end()    { return iterator(this, (std::numeric_limits<position_type>::max)()); }
    reverse_iterator rbegin() { return reverse_iterator(this->begin()); }
    reverse_iterator rend()   { return reverse_iterator(this->end()); }

    bool expand() {
        BOOST_STATIC_CONSTANT(std::size_t, N = 1024);
        char_type buffer[N];
        this->stream_.read(buffer, N);
        this->buffer_.insert(this->buffer_.end(), buffer, buffer + this->stream_.gcount());
        return 0 < this->stream_.gcount();
    }

    position_type read_all() {
        this->buffer_.insert(this->buffer_.end(),
                             std::istreambuf_iterator<char_type>(this->stream_),
                             std::istreambuf_iterator<char_type>());
        return this->buffer_.size();
    }

    char_type     get(position_type const index) { return this->buffer_[index]; }
    position_type current_size()    const { return this->buffer_.size(); }

  private:

    input_stream_type&     stream_;
    std::vector<char_type> buffer_;
};

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_TEMPLATES_DETAIL_HPP_INCLUDED
