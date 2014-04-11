//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

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

    typedef typename base_type::traits_type                                     traits_type;
    typedef typename base_type::char_type                                       char_type;
    typedef typename base_type::size_type                                       size_type;
    typedef typename base_type::boolean_type                                    boolean_type;
    typedef typename base_type::string_type                                     string_type;
    typedef string_type                                                         path_type;
    typedef std::vector<path_type>                                              paths_type;
    typedef std::pair<path_type, size_type>                                     info_type;

  public:

    file_template( path_type  const& path
                 , paths_type const& directories = paths_type(/*1, "."*/)
                 )
        : base_member_type(make_iterator(path, directories))
        , base_type( base_member_type::member
                   , base_member_type::member ? base_member_type::member.make_end() : base_member_type::member
                     // This chicken dance is needed because spirit's file_iterator can't handle empty files.
                   , boolean_type(base_member_type::member)
                   )
        , path_(path) {} // TODO: Use info.first instead.

  public:

    path_type const& path() const { return path_; }

  private:

    inline static Iterator make_iterator( path_type  const& path
                                        , paths_type const& directories
                                        ) {
        info_type const& info = locate_file(path, directories);
        return info.second == 0 ? Iterator() : Iterator(traits_type::narrow(info.first));
    }

    inline static info_type locate_file( path_type  const& path
                                       , paths_type const& directories
                                       ) {
        struct stat file;
        namespace algo = boost::algorithm;

        // First try looking in the directories specified.
        BOOST_FOREACH(path_type const& directory, directories) {
            path_type const& base = algo::trim_right_copy_if(directory, algo::is_any_of("/"));
            path_type const& full = base + char_type('/') + path;
            if (stat(traits_type::narrow(full).c_str(), &file) == 0) { // Found it.
                return info_type(full, file.st_size);
            }
        }

        std::string const narrow_path = traits_type::narrow(path);

        // Then try the current directory.
        if (stat(narrow_path.c_str(), &file) != 0) {
            throw_exception(file_error(narrow_path, "read", std::strerror(errno)));
        }

        return info_type(path, file.st_size);
    }

    /*
    inline static std::string const& check_exists(std::string const& path) {
        struct stat file;

        if (stat(path.c_str(), &file) != 0) {
            throw_exception(file_error(path, "read", std::strerror(errno)));
        }
        else if (file.st_size == 0) {
            throw_exception(file_error(path, "read", "file is empty"));
        }

        // Using fopen:
        // if (FILE *const file = std::fopen(filename, "rb")) {
        //     std::fclose(file);
        // }
        // else { throw }

        // Using access:
        // if (access(path.c_str(), R_OK | F_OK) != 0) {
        //    throw_exception(file_error(path, "read", std::strerror(errno)));
        // }

        return path;
    }
    */

  private:

    path_type const path_;
};

template < class Char
         , class Engine
         >
struct file_template_identity : boost::mpl::identity<file_template<Char, Engine> > {};

}} // namespace ajg::synth

#endif // AJG_SYNTH_TEMPLATES_FILE_TEMPLATE_HPP_INCLUDED
