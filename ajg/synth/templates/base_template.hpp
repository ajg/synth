//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_BASE_TEMPLATE_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_BASE_TEMPLATE_HPP_INCLUDED

#include <map>
#include <string>
#include <cerrno>
#include <fstream>
#include <sstream>
#include <utility>
#include <stdexcept>

#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/throw_exception.hpp>

#include <ajg/synth/engines/detail.hpp>
#include <ajg/synth/engines/exceptions.hpp>

namespace ajg {
namespace synth {

using boost::throw_exception;

template < class Engine
         , class Iterator
         >
struct base_template : boost::noncopyable {
  public:

    typedef typename Engine::template definition<Iterator>                      engine_type;
    typedef typename engine_type::char_type                                     char_type;
    typedef typename engine_type::size_type                                     size_type;
    typedef typename engine_type::sequence_type                                 sequence_type;
    typedef typename engine_type::value_type                                    value_type;
    typedef typename engine_type::string_type                                   string_type;
    typedef typename engine_type::stream_type                                   stream_type;
    typedef typename engine_type::frame_type                                    frame_type;
    typedef typename engine_type::context_type                                  context_type;
    typedef typename engine_type::options_type                                  options_type;
    typedef typename engine_type::iterator_type                                 iterator_type;
    typedef typename value_type::traits_type                                    traits_type;
    typedef std::pair<iterator_type, iterator_type>                             range_type;

  public:

    base_template( iterator_type const& begin
                 , iterator_type const& end
              // ,  bool const cache = false
                 )
            : engine_(&shared_engine()) {
        reset(begin, end);
    }

  public:

//
// render
////////////////////////////////////////////////////////////////////////////////////////////////////

    void render( stream_type&        stream
               , context_type const& context = context_type()
               , options_type const& options = options_type()
               ) const {
        engine_->render(stream, frame_, context, options);
    }

//
// Convenience methods
////////////////////////////////////////////////////////////////////////////////////////////////////

    string_type render_to_string( context_type const& context = context_type()
                                , options_type const& options = options_type()
                                ) const {
        std::basic_ostringstream<char_type> stream;
        engine_->render(stream, frame_, context, options);
        return stream.str();
    }

    void render_to_file( string_type  const& filepath
                       , context_type const& context = context_type()
                       , options_type const& options = options_type()
                       ) const {
        std::string const path = traits_type::narrow(filepath);
        std::basic_ofstream<char_type> file;

        try {
            file.open(path.c_str(), std::ios::binary);
        }
        catch (std::exception const& e) {
            throw_exception(file_error(filepath, "write", e.what()));
        }

        engine_->render(file, frame_, context, options);
    }

    range_type& range() const { return range_; }

    string_type text() const {
        return string_type(range_.first, range_.second);
    }

  protected:

//
// reset
////////////////////////////////////////////////////////////////////////////////////////////////////

    void reset( iterator_type const& begin
              , iterator_type const& end
              ) {
        range_ = range_type(begin, end);
        if (begin == end) {
            frame_ = frame_type();
        }
        else {
            engine_->parse(begin, end, frame_);
        }
    }

  private:

    inline static engine_type& shared_engine() {
        static engine_type engine;
        return engine;
    }

    void mutate_locally() {
        if (!local_engine_) {
            local_engine_.reset(new engine_type(shared_engine()));
            std::swap(engine_, local_engine_.get());
        }
    }

  private:

    boost::scoped_ptr<engine_type>      local_engine_;
    engine_type*                        engine_;
    frame_type                          frame_;
    range_type                          range_;
};


#if AJG_SYNTH_OBSOLETE

template <class Char>
std::basic_string<Char> read_file(std::basic_string<Char> const& filepath) const {
    std::string const path = traits_type::narrow(filepath);
    std::basic_ifstream<Char> file;

    try {
        file.open(path.c_str(), std::ios::binary);
        return read_stream<std::basic_string<Char> >(file);
    }
    catch (std::exception const& e) {
        throw_exception(file_error(path, "read", e.what()));
    }
}

template <class String, class Stream>
inline String read_stream
        ( Stream& stream
        , optional<typename Stream::size_type> const size = none
        ) {
    BOOST_STATIC_CONSTANT(typename String::size_type, buffer_size = 4096);

    if (!stream.good()) {
        throw_exception(std::runtime_error("bad stream"));
    }

    String result;
    if (size) result.reserve(*size);
    typename String::value_type buffer[buffer_size];

    while (!stream.eof()) {
        stream.read(buffer, buffer_size);
        result.append(buffer, stream.gcount());
    }

    if (stream.bad()) {
        throw_exception(std::runtime_error("bad stream"));
    }

    return result;
}

#endif // AJG_SYNTH_OBSOLETE

}} // namespace ajg::synth

#endif // AJG_SYNTH_TEMPLATES_BASE_TEMPLATE_HPP_INCLUDED
