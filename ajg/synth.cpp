//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <cstdlib>

#include <ajg/synth.hpp>
#include <ajg/synth/command.hpp>
#include <ajg/synth/processor.hpp>

namespace {

namespace synth = ajg::synth;

typedef synth::processor<synth::detail::multi_template
    < char
    , synth::stream_template_identity
    , synth::django::engine<>
    , synth::ssi::engine<>
    , synth::tmpl::engine<>
    >
> processor_type;

} // namespace

namespace boost {
namespace property_tree {

processor_type::stream_type& operator <<( processor_type::stream_type&        output
                                        , processor_type::context_type const& context
                                        ) {
    return output << "[context]";
}

}} // namespace boost::property_tree

int main(int const argc, char const *const argv[])
try {
    ajg::synth::command<processor_type>::process(argc, argv);
    return EXIT_SUCCESS;
} catch (std::exception const& e) {
    std::cerr << "synth: " << e.what() << std::endl;
    return EXIT_FAILURE;
}
