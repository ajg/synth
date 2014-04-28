//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_STANDARD_ENVIRONMENT_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_STANDARD_ENVIRONMENT_HPP_INCLUDED

#include <ajg/synth/support.hpp>

#include <boost/program_options/environment_iterator.hpp>

#if !AJG_SYNTH_IS_PLATFORM_WINDOWS
#    include <unistd.h>
extern char **environ;
#endif

namespace ajg {
namespace synth {
namespace detail {

//
// standard_environment:
//     Safer and iterable interface to the program's environment.
////////////////////////////////////////////////////////////////////////////////////////////////////

struct standard_environment {
  public:

    typedef boost::environment_iterator         iterator;
    typedef boost::environment_iterator         const_iterator;
    typedef iterator::value_type                value_type;
    typedef value_type::first_type              key_type;
    typedef value_type::second_type             mapped_type;

  public:

    const_iterator begin() const {
        return const_iterator(environ);
    }

    const_iterator end() const {
        return const_iterator();
    }

    const_iterator find(key_type const& name) const {
        const_iterator const end = this->end();

        for (const_iterator it = begin(); it != end; ++it) {
            if (it->first == name) {
                return it;
            }
        }

        return end;
    }
};

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_STANDARD_ENVIRONMENT_HPP_INCLUDED
