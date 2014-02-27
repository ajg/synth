//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTHESIS_TEMPLATES_FILE_TEMPLATE_HPP_INCLUDED
#define AJG_SYNTHESIS_TEMPLATES_FILE_TEMPLATE_HPP_INCLUDED

#include <string>
#include <vector>
#include <cstring>
#include <sys/stat.h>

#ifdef AJG_SYNTHESIS_NO_WINDOWS_H
  #ifndef BOOST_SPIRIT_FILEITERATOR_STD
  // Prevents the inclusion of <windows.h>:
    #define BOOST_SPIRIT_FILEITERATOR_STD
  #endif // !BOOST_SPIRIT_FILEITERATOR_STD
#endif // AJG_SYNTHESIS_NO_WINDOWS_H

#include <boost/throw_exception.hpp>
#include <boost/utility/base_from_member.hpp>
#include <boost/spirit/include/classic_file_iterator.hpp>

#include <ajg/synthesis/engines/exceptions.hpp>
#include <ajg/synthesis/templates/base_template.hpp>

namespace ajg {
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

    typedef std::string                filepath_type;
    typedef std::vector<filepath_type> directories_type;

  public:

    file_template( filepath_type    const& filepath
                 , directories_type const& directories = directories_type(/*1, "."*/)
                 )
        : base_from_member<Iterator>(find_path(filepath, directories)) // (check_exists(filepath))
        , base_type(this->member, this->member.make_end())
        , filepath_(filepath) {}

  public:

    filepath_type const& filepath() const { return filepath_; }

  private:

    inline static filepath_type find_path( filepath_type    const& filepath
                                         , directories_type const& directories
                                         ) {
        struct stat file;

        // First try looking in the directories specified.
        BOOST_FOREACH(filepath_type const& directory, directories) {
            filepath_type const& path = directory + filepath;
            if (stat(path.c_str(), &file) == 0) {
                return path;
            }
        }

        // Then try the current directory.
        if (stat(filepath.c_str(), &file) != 0) {
            throw_exception(file_error(filepath, "read", std::strerror(errno)));
        }
        else if (file.st_size == 0) {
            throw_exception(file_error(filepath, "read", "file is empty"));
        }

        return filepath;
    }

    /*
    inline static filepath_type const& check_exists(filepath_type const& filepath) {
        struct stat file;

        if (stat(filepath.c_str(), &file) != 0) {
            throw_exception(file_error(filepath, "read", std::strerror(errno)));
        }
        else if (file.st_size == 0) {
            throw_exception(file_error(filepath, "read", "file is empty"));
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
    */

  private:

    filepath_type const filepath_;
};

}} // namespace ajg::synthesis

#endif // AJG_SYNTHESIS_TEMPLATES_FILE_TEMPLATE_HPP_INCLUDED
