
#include <iostream>
#include <ajg/synth.hpp>

namespace synth = ajg::synth;

typedef synth::default_traits<char>             traits_type;
typedef synth::ssi::engine<traits_type>         engine_type;
typedef synth::string_template<engine_type>     template_type;

int main() {
    template_type const tpl("Howdy, <!--#echo var=\"user\" -->!");
    template_type::context_type ctx;
    ctx["user"] = "Dolph Lundgren";

    // Render to different targets:
    tpl.render(std::cout);
    tpl.render_to_path("greeting.txt", ctx);
    std::cout << tpl.render_to_string(ctx);
    return 0;
}
