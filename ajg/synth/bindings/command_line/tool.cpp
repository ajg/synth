//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#include <ajg/synth/support.hpp> // Must come ahead of everything else.

#include <cstdlib>
#include <iostream>

#include <ajg/synth/bindings/command_line/command.hpp>
#include <ajg/synth/bindings/command_line/binding.hpp>


namespace {

namespace s = ajg::synth;

typedef s::default_traits<AJG_SYNTH_CONFIG_DEFAULT_CHAR_TYPE>                   traits_type;
typedef s::bindings::command_line::binding<traits_type>                         binding_type;
typedef s::bindings::command_line::command<binding_type>                        command_type;

} // namespace


namespace boost {
namespace property_tree {

binding_type::ostream_type& operator <<( binding_type::ostream_type&       ostream
                                       , binding_type::context_type const& context
                                       ) {
    return ostream << "[context]";
}

}} // namespace boost::property_tree

int main(int const argc, char const* argv[])
try {
    command_type::run(argc - 1, argv + 1);
    return EXIT_SUCCESS;
}
catch (std::exception const& e) {
    std::cerr << "synth: " << e.what() << std::endl;
    return EXIT_FAILURE;
}
