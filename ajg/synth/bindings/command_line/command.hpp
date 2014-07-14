//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_COMMAND_LINE_COMMAND_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_COMMAND_LINE_COMMAND_HPP_INCLUDED

#include <ajg/synth/support.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <external/other/optionparser.h>

#include <ajg/synth/exceptions.hpp>
#include <ajg/synth/detail/text.hpp>

namespace ajg {
namespace synth {
namespace bindings {
namespace command_line {

enum command_options
    { unknown_option
    , help_option
    , version_option
    , context_option
    , engine_option
    , directory_option
    };

template <class Binding>
// TODO: Fold this into binding.
struct command {
  public:

    typedef command                                                             command_type;
    typedef Binding                                                             binding_type;

    typedef typename binding_type::foreign_type                                 ptree_type;
    typedef typename binding_type::options_type                                 options_type;
    typedef typename binding_type::traits_type                                  traits_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::istream_type                                  istream_type;
    typedef typename traits_type::ostream_type                                  ostream_type;
    typedef typename traits_type::paths_type                                    paths_type;

  private:

    typedef ::option::ArgStatus                                                 status_type;
    typedef ::option::Descriptor                                                descriptor_type;
    typedef ::option::Option                                                    option_type; // Unrelated to options_type.
    typedef ::option::Parser                                                    parser_type;
    typedef detail::text<string_type>                                           text;

  public:

    static void run(int const argc, char_type const* argv[]) {
        static descriptor_type const descriptors[] =
            { {unknown_option,     0, "",  ""    ,        param_illegal,  "USAGE: synth [OPTIONS...]\nOptions:"}
            , {help_option,        0, "h", "help",        param_illegal,  "  -h,      --help              print help message"}
            , {version_option,     0, "v", "version",     param_illegal,  "  -v,      --version           print library version"}
            , {context_option,     0, "c", "context",     param_required, "  -c file, --context=file      contextual data             *.{ini,json,xml}"}
            , {engine_option,      0, "e", "engine",      param_required, "  -e name, --engine=name       template engine             {django,ssi,tmpl}"}
            , {directory_option,   0, "d", "directory",   param_required, "  -d path, --directory=path    template location(s)        (default: '.')"}
            , {unknown_option,     0, "",  "",            param_allowed,  "\n"}
            // ("input,i",       ("file", string),  "the source (default: '-')")            // TODO
            // ("output,o",      ("file", string),  "the destination (default: '-')")       // TODO
            // ("source,s",      ("text", string),  "inline alternative to input file")     // TODO
            // ("?,?",           ("name", string),  "the context's format: {ini,json,xml}") // TODO
            // TODO: formats
            , {0, 0, 0, 0, 0, 0}
            };

        size_type const n = sizeof(descriptors) / sizeof(descriptors[0]);
        option_type opts[n], buffer[n];
        parser_type parser(descriptors, argc, argv, opts, buffer);

        if (parser.error()) {
            AJG_SYNTH_THROW(std::runtime_error("command option parsing"));
        }

        istream_type& input  = std::cin;
        ostream_type& output = std::cout;
        ostream_type& error  = std::cerr;

        for (int i = 0; i < parser.nonOptionsCount(); ++i) {
            AJG_SYNTH_THROW(synth::unknown_argument(parser.nonOption(i)));
        }

        for (option_type* option = opts[unknown_option]; option; option = option->next()) {
            AJG_SYNTH_THROW(synth::unknown_option(option->name));
        }

        if (opts[help_option]/* || TODO: (argc == 0 && input is stdin and empty [non-blocking])*/) {
            ::option::printUsage(error, descriptors);
            return;
        }
        else if (opts[version_option]) {
            output << "synth v" << AJG_SYNTH_VERSION_STRING << std::endl;
            return;
        }
        else if (!opts[engine_option]) {
            AJG_SYNTH_THROW(missing_option("engine"));
        }

        paths_type directories;
        for (option_type* option = opts[directory_option]; option; option = option->next()) {
            directories.push_back(to_string(option));
        }

        options_type options;
        // TODO: options.metadata.
        options.debug       = false; // TODO: Turn into a flag.
        options.directories = directories;
        options.caching     = engines::no_caching;

        string_type  const engine = to_string(opts[engine_option].last());
        binding_type const binding(input >> std::noskipws, engine, options);

        ptree_type ptree;
        if (option_type const* const option = opts[context_option].last()) {
            std::string const narrow_path = option->arg;
            std::basic_ifstream<char_type> file;

            try {
                file.open(narrow_path.c_str(), std::ios::binary);
            }
            catch (std::exception const& e) {
                AJG_SYNTH_THROW(read_error(narrow_path, e.what()));
            }

                 if (text::ends_with(narrow_path, ".ini"))  boost::property_tree::read_ini(file, ptree);
            else if (text::ends_with(narrow_path, ".json")) boost::property_tree::read_json(file, ptree);
            else if (text::ends_with(narrow_path, ".xml"))  boost::property_tree::read_xml(file, ptree);
            else AJG_SYNTH_THROW(invalid_parameter(name_of(*option)));
        }

        binding.render_to_stream(output, ptree);
    }

  private:

    inline static string_type to_string(option_type const* option) {
        if (option == 0 || option->arg == 0) return string_type();
        else return text::widen(std::string(option->arg));
    }

    inline static boolean_type to_boolean(option_type const* option) {
        if (option == 0 || option->arg == 0) return true;
        else {
            std::string const parameter(option->arg);
                 if (parameter == "true"  || parameter == "yes" || parameter == "1") return true;
            else if (parameter == "false" || parameter == "no"  || parameter == "0") return false;
            else AJG_SYNTH_THROW(invalid_parameter(name_of(*option)));
        }
    }

    inline static std::string name_of(option_type const& option) {
        std::string const name(option.name, option.namelen);
        return option.name[option.namelen] == 0 ? "-" + name : "--" + name;
    }

    static status_type param_illegal(option_type const& option, bool const) {
        if (option.arg != 0) AJG_SYNTH_THROW(superfluous_parameter(name_of(option)));
        else return ::option::ARG_NONE;
    }

    static status_type param_allowed(option_type const& option, bool const) {
             if (option.arg == 0)                  return ::option::ARG_NONE;
     // else if (option.name[option.namelen] == 0) return ::option::ARG_IGNORE; // i.e. a shortopt
        else if (option.arg[0] == 0) AJG_SYNTH_THROW(empty_parameter(name_of(option)));
        else return ::option::ARG_OK;
    }

    static status_type param_required(option_type const& option, bool const) {
             if (option.arg == 0)    AJG_SYNTH_THROW(missing_parameter(name_of(option)));
        else if (option.arg[0] == 0) AJG_SYNTH_THROW(empty_parameter(name_of(option)));
        else return ::option::ARG_OK;
    }
};

}}}} // namespace ajg::synth::bindings::command_line

#endif // AJG_SYNTH_BINDINGS_COMMAND_LINE_COMMAND_HPP_INCLUDED

