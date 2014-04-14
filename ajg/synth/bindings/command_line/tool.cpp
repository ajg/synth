//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#include <ajg/synth/config.hpp> // Must come ahead of everything else.

#include <cstdlib>
#include <iostream>

#include <ajg/synth/engines.hpp>
#include <ajg/synth/adapters.hpp>
#include <ajg/synth/templates.hpp>
#include <ajg/synth/bindings/command_line/command.hpp>
#include <ajg/synth/bindings/command_line/binding.hpp>


namespace {

namespace synth = ajg::synth;

typedef synth::default_traits<AJG_SYNTH_DEFAULT_CHAR_TYPE> traits_type;
typedef synth::command_line::binding<synth::detail::multi_template
    < traits_type
    , synth::stream_template
    , synth::django::engine<traits_type>
    , synth::ssi::engine<traits_type>
    , synth::tmpl::engine<traits_type>
    >
> binding_type;

typedef synth::command_line::command<binding_type> command_type;

} // namespace

namespace boost {
namespace property_tree {

binding_type::ostream_type& operator <<( binding_type::ostream_type&       ostream
                                       , binding_type::context_type const& context
                                       ) {
    return ostream << "[context]";
}

}} // namespace boost::property_tree

int main(int const argc, char const *const argv[])
try {
    command_type::run(argc, argv);
    return EXIT_SUCCESS;
}
catch (std::exception const& e) {
    std::cerr << "synth: " << e.what() << std::endl;
    return EXIT_FAILURE;
}
