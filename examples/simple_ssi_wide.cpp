
#include <iostream>
#include <ajg/synth.hpp>

int wmain() {
    using namespace ajg::synth;

    typedef string_template<wchar_t, ssi::engine<> > template_type;
    template_type const tpl(L"Howdy, <!--#echo var=\"user\" -->!");
    template_type::context_type ctx;
    ctx[L"user"] = L"Dolph Lundgren";

    // Render to different targets:
    tpl.render(std::wcout);
    tpl.render_to_file(L"greeting.txt", ctx);
    std::wcout << tpl.render_to_string(ctx);
    return 0;
}
