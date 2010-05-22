
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_TEMPLATES_FILE_TEMPLATE_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_TEMPLATES_FILE_TEMPLATE_HPP_INCLUDED

#include <string>
#include <sys/stat.h>

#ifdef CHEMICAL_SYNTHESIS_NO_WINDOWS_H
  #ifndef BOOST_SPIRIT_FILEITERATOR_STD
  // Prevents the inclusion of <windows.h>:
    #define BOOST_SPIRIT_FILEITERATOR_STD
  #endif // !BOOST_SPIRIT_FILEITERATOR_STD
#endif // CHEMICAL_SYNTHESIS_NO_WINDOWS_H

#include <boost/throw_exception.hpp>
#include <boost/utility/base_from_member.hpp>
#include <boost/spirit/include/classic_file_iterator.hpp>

#include <chemical/synthesis/engines/exceptions.hpp>
#include <chemical/synthesis/templates/base_template.hpp>

namespace chemical {
namespace synthesis {

template < class Char
         , class Engine
         , class Iterator = spirit::classic::file_iterator<Char>
         >
struct file_template
    : private base_from_member<Iterator>
    , public  base_template<Engine, Iterator> {

  private:

    typedef base_template<Engine, Iterator> base_type;

  public:

    file_template(std::string const& filepath)
        : base_from_member<Iterator>(check_exists(filepath))
        , base_type(this->member, this->member.make_end())
        , filepath_(filepath) {}

  public:

    std::string const& filepath() const { return filepath_; }

  private:

    inline static std::string const& check_exists(std::string const& filepath) {
        struct stat file;

        if (stat(filepath.c_str(), &file) != 0) {
            throw_exception(file_error(filepath, "read", std::strerror(errno)));
        }
        else if (file.st_size == 0) {
            throw_exception(file_error(filepath, "read", "File is empty"));
        }

        // Using fopen:
        // if (FILE *const file = std::fopen(filename, "rb")) {
        //     std::fclose(file);
        // }
        // else { throw }

        // Using access:
        // if (access(filepath.c_str(), R_OK | F_OK) != 0) {
        //    throw_exception(file_error(filepath, "read", std::strerror(errno)));
        // }

        return filepath;
    }

  private:

    std::string const filepath_;
};

}} // namespace chemical::synthesis

#endif // CHEMICAL_SYNTHESIS_TEMPLATES_FILE_TEMPLATE_HPP_INCLUDED
