//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_COMMAND_LINE_COMMAND_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_COMMAND_LINE_COMMAND_HPP_INCLUDED

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace ajg {
namespace synth {
namespace command_line {

#define AJG_ARG(name, type) po::value<type##_type>()->value_name(name)

template <class Binding>
struct command {

  public:

    typedef Binding                                   binding_type;
    typedef typename binding_type::char_type        char_type;
    typedef typename binding_type::boolean_type     boolean_type;
    typedef typename binding_type::string_type      string_type;
    typedef typename binding_type::stream_type      stream_type;
    typedef typename binding_type::context_type     context_type;
    typedef typename binding_type::directories_type dirs_type;

  public:

    static void run(int const argc, char_type const *const argv[]) {
        namespace po = boost::program_options;
        namespace pt = boost::property_tree;

        po::options_description description("Usage");

        description.add_options()
            ("help,h",                                  "print help message")
            ("version,v",                               "print library version")
         // ("input,i",       AJG_ARG("file", string),  "the source (default: '-')")            // TODO
         // ("output,o",      AJG_ARG("file", string),  "the destination (default: '-')")       // TODO
         // ("source,s",      AJG_ARG("text", string),  "inline alternative to input file")     // TODO
         // ("format,f",      AJG_ARG("name", string),  "the context's format: {ini,json,xml}") // TODO
            ("context,c",     AJG_ARG("file", string),  "the data: *.{ini,json,xml}")
            ("engine,e",      AJG_ARG("name", string),  "template engine: {django,ssi,tmpl}")
            ("autoescape,a",  AJG_ARG("bool", boolean), "automatically escape values (default: 'true')")
            ("directories,d", AJG_ARG("path", dirs),    "template lookup directories (default: '.')")
            ("replacement,r", AJG_ARG("text", string),  "replaces missing values (default: '')")
            ;

        po::variables_map flags;
        po::store(po::parse_command_line(argc, argv, description), flags);
        po::notify(flags);

        if (flags.count("help")) {
            std::cerr << description << std::endl;
            return;
        } else if (flags.count("version")) {
            std::cerr << "synth v" << AJG_SYNTH_VERSION_STRING << std::endl;
            return;
        } else if (!flags.count("engine")) {
            throw_exception(std::invalid_argument("missing engine name"));
        }

        binding_type const binding
            ( std::cin >> std::noskipws
            , flags["engine"].as<string_type>()
            , flags.count("autoescape")  ? flags["autoescape"].as<boolean_type>() : boolean_type(true)
            , flags.count("replacement") ? flags["replacement"].as<string_type>() : string_type()
            , flags.count("directories") ? flags["directories"].as<dirs_type>()   : dirs_type()
            );

        context_type context;

        if (flags.count("context")) {
            string_type const& filename = flags["context"].as<string_type>();
            std::basic_ifstream<char_type> file(filename);

            if (boost::algorithm::ends_with(filename, synth::detail::text(".ini"))) {
                pt::read_ini(file, context);
            }
            else if (boost::algorithm::ends_with(filename, synth::detail::text(".json"))) {
                pt::read_json(file, context);
            }
            else if (boost::algorithm::ends_with(filename, synth::detail::text(".xml"))) {
                pt::read_xml(file, context);
            }
            else {
                throw_exception(std::invalid_argument("unknown context format"));
            }
        }

        binding.render(std::cout, context);
    }
};

#undef AJG_ARG

}}} // namespace ajg::synth::command_line

#endif // AJG_SYNTH_BINDINGS_COMMAND_LINE_COMMAND_HPP_INCLUDED

