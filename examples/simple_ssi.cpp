
#include <iostream>
#include <ajg/synth.hpp>

namespace synth = ajg::synth;

typedef synth::default_traits<char>                         traits_type;
typedef synth::engines::ssi::engine<traits_type>            engine_type;
typedef synth::templates::string_template<engine_type>      template_type;

int main() {
    template_type const t("Howdy, <!--#echo var=\"user\" -->!");
    template_type::context_type c;
    c["user"] = "Dolph Lundgren";

    // Render to different destinations:
    t.render_to_stream(std::cout);
    t.render_to_path("greeting.txt", c);
    std::cout << t.render_to_string(c);
    return 0;
}
