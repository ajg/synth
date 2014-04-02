//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_FILE_TEMPLATE_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_FILE_TEMPLATE_HPP_INCLUDED

#include <string>
#include <vector>
#include <cstring>
#include <sys/stat.h>

#ifdef AJG_SYNTH_NO_WINDOWS_H
  #ifndef BOOST_SPIRIT_FILEITERATOR_STD
  // Prevents the inclusion of <windows.h>:
    #define BOOST_SPIRIT_FILEITERATOR_STD
  #endif // !BOOST_SPIRIT_FILEITERATOR_STD
#endif // AJG_SYNTH_NO_WINDOWS_H

#include <boost/foreach.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/throw_exception.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/utility/base_from_member.hpp>
#include <boost/spirit/include/classic_file_iterator.hpp>

#include <ajg/synth/detail.hpp>
#include <ajg/synth/templates/base_template.hpp>

namespace ajg {
namespace synth {

template < class Char
         , class Engine
         , class Iterator = boost::spirit::classic::file_iterator<Char>
         >
struct file_template
    : private boost::base_from_member<Iterator >
    , public  base_template<Engine, Iterator> {

  private:

    typedef boost::base_from_member<Iterator>                                   base_member_type;
    typedef base_template<Engine, Iterator>                                     base_type;

  public:

    typedef typename base_type::size_type       size_type;
    typedef typename base_type::boolean_type    boolean_type;
    typedef std::string                         filepath_type;
    typedef std::vector<filepath_type>          directories_type;
    typedef std::pair<filepath_type, size_type> info_type;

  public:

    file_template( filepath_type    const& filepath
                 , directories_type const& directories = directories_type(/*1, "."*/)
                 )
        : base_member_type(make_iterator(filepath, directories))
        , base_type( base_member_type::member
                   , base_member_type::member ? base_member_type::member.make_end() : base_member_type::member
                     // This chicken dance is needed because file_iterator can't handle empty files.
                   , boolean_type(base_member_type::member)
                   )
        , filepath_(filepath) {} // TODO: Use info.first instead.

  public:

    filepath_type const& filepath() const { return filepath_; }

  private:

    inline static Iterator make_iterator( filepath_type    const& filepath
                                        , directories_type const& directories
                                        ) {
        info_type const& info = locate_file(filepath, directories);
        return info.second == 0 ? Iterator() : Iterator(info.first);
    }

    inline static info_type locate_file( filepath_type    const& filepath
                                       , directories_type const& directories
                                       ) {
        struct stat file;
        namespace algo = boost::algorithm;

        // First try looking in the directories specified.
        BOOST_FOREACH(filepath_type const& directory, directories) {
            filepath_type const& base = algo::trim_right_copy_if(directory, algo::is_any_of("/"));
            filepath_type const& path = base + "/" + filepath;
            if (stat(path.c_str(), &file) == 0) { // Found it.
                return info_type(path, file.st_size);
            }
        }

        // Then try the current directory.
        if (stat(filepath.c_str(), &file) != 0) {
            throw_exception(file_error(filepath, "read", std::strerror(errno)));
        }

        return info_type(filepath, file.st_size);
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

template < class Char
         , class Engine
         >
struct file_template_identity : boost::mpl::identity<file_template<Char, Engine> > {};

}} // namespace ajg::synth

#endif // AJG_SYNTH_TEMPLATES_FILE_TEMPLATE_HPP_INCLUDED
