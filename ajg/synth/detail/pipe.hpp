//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_PIPE_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_PIPE_HPP_INCLUDED

#include <ajg/synth/support.hpp>

#include <boost/noncopyable.hpp>

#include <ajg/synth/detail/filesystem.hpp>

namespace ajg {
namespace synth {
namespace detail {

//
// pipe:
//     Nicer, safer interface to popen/pclose.
////////////////////////////////////////////////////////////////////////////////////////////////////

struct pipe : boost::noncopyable {
  public:

    explicit pipe(std::string const& command, bool const reading = true) {
        if ((file_ = AJG_SYNTH_IF_MSVC(_popen, popen)(command.c_str(), reading ? "r" : "w")) == 0) {
            AJG_SYNTH_THROW(error("open"));
        }
    }

    ~pipe() {
        if (AJG_SYNTH_IF_MSVC(_pclose, pclose)(file_) == -1) {
            AJG_SYNTH_THROW(error("close"));
        }
    }

  public:

    // TODO: Move this into exceptions.hpp
    struct error : public std::runtime_error {
        error(std::string const& action)
            : std::runtime_error("could not " + action + " pipe (" + std::strerror(errno) + ")") {}
    };

  public:

    template <class Stream>
    void read_into(Stream& stream) {
        read_file_to_stream(file_, stream);
    }

  private:

    FILE* file_;
};
}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_PIPE_HPP_INCLUDED
