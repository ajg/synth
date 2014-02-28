//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>

#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>

#include <ajg/synth.hpp>


int main(int const argc, char const *const argv[]) {
    // TODO: Parameterize on char type.
    namespace po = boost::program_options;
    po::options_description description("Usage");
    typedef std::vector<std::string> paths;

    description.add_options()
        ("help,h",                                                       "print help message")
        ("version,v",                                                    "print library version")
        ("input,i",       po::value<std::string>()->value_name("file"),  "template input (default: '-')")
        ("output,o",      po::value<std::string>()->value_name("file"),  "template output (default: '-')")
        ("context,c",     po::value<std::string>()->value_name("file"),  "template context: *.{ini,json,xml}")
        ("engine,e",      po::value<std::string>()->value_name("name"),  "template engine: {django,ssi,tmpl}")
        ("autoescape,a",  po::value<bool>()->value_name("bool"),         "automatically escape values (default: 'true')")
        ("directories,d", po::value<paths>()->value_name("path"),        "template lookup directories (default: '.')")
        ("replacement,r", po::value<std::string>()->value_name("text"),  "replaces missing values (default: '')")
        ;

    po::variables_map flags;
    po::store(po::parse_command_line(argc, argv, description), flags);
    po::notify(flags);

    if (flags.count("help")) {
        std::cout << description << std::endl;
        return EXIT_SUCCESS;
    } else if (flags.count("version")) {
        std::cout << "synth v" << AJG_SYNTH_VERSION_STRING << std::endl;
        return EXIT_SUCCESS;
    }


    return EXIT_SUCCESS;
