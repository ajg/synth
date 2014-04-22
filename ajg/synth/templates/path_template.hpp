//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_PATH_TEMPLATE_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_PATH_TEMPLATE_HPP_INCLUDED

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
#include <boost/throw_exception.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/spirit/include/classic_file_iterator.hpp>

#include <ajg/synth/detail.hpp>
#include <ajg/synth/templates/base_template.hpp>

namespace ajg {
namespace synth {
namespace templates {

template <class Engine>
struct path_template : base_template< Engine
                                    , boost::spirit::classic::file_iterator<typename Engine::char_type>
                                    > {
  public:

    typedef path_template                                                       template_type;
    typedef Engine                                                              engine_type;
    typedef typename path_template::kernel_type                                 kernel_type;
    typedef typename kernel_type::iterator_type                                 iterator_type;
    typedef typename kernel_type::range_type                                    range_type;
    typedef typename engine_type::traits_type                                   traits_type;

    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::path_type                                     path_type;
    typedef typename traits_type::paths_type                                    paths_type;

    typedef std::pair<path_type, size_type>                                     info_type;

  public:

    path_template(path_type const& path, paths_type const& directories = paths_type())
            : info_(locate_file(path, directories)) {
        if (boolean_type const empty_file = this->info_.second == 0) {
            this->reset();
        }
        else {
            iterator_type begin(traits_type::narrow(this->info_.first));
            iterator_type end = begin ? begin.make_end() : iterator_type();
            this->reset(begin, end);
        }
    }

  private:

    inline static info_type locate_file(path_type const& path, paths_type const& directories) {
        struct stat stats;
        namespace algo = boost::algorithm;

        // First try looking in the directories specified.
        BOOST_FOREACH(path_type const& directory, directories) {
            path_type const& base = algo::trim_right_copy_if(directory, algo::is_any_of("/"));
            path_type const& full = base + char_type('/') + path;
            if (stat(traits_type::narrow(full).c_str(), &stats) == 0) { // Found it.
                return info_type(full, stats.st_size);
            }
        }

        std::string const narrow_path = traits_type::narrow(path);

        // Then try the current directory.
        if (stat(narrow_path.c_str(), &stats) != 0) { // TODO: Use wstat where applicable.
            throw_exception(file_error(narrow_path, "read", std::strerror(errno)));
        }

        return info_type(path, stats.st_size);
    }

    /*
    inline static std::string const& check_exists(std::string const& path) {
        struct stat stats;

        if (stat(path.c_str(), &stats) != 0) {
            throw_exception(file_error(path, "read", std::strerror(errno)));
        }
        else if (stats.st_size == 0) {
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

  public:

    info_type const& info() const { return this->info_; }

  private:

    info_type const info_;
};

}}} // namespace ajg::synth::templates

#endif // AJG_SYNTH_TEMPLATES_PATH_TEMPLATE_HPP_INCLUDED
