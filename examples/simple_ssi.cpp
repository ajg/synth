
#include <iostream>
#include <ajg/synth.hpp>

int main() {
    using namespace ajg::synth;

    typedef string_template<char, ssi::engine<> > template_type;
    template_type const tpl("Howdy, <!--#echo var=\"user\" -->!");
    template_type::context_type ctx;
    ctx["user"] = "Dolph Lundgren";

    // Render to different targets:
    tpl.render(std::cout);
    tpl.render_to_file("greeting.txt", ctx);
    std::cout << tpl.render_to_string(ctx);
    return 0;
}
